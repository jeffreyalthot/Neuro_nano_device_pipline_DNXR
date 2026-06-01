#include "../src/replay/ReplayPlayer.h"
#include "../src/replay/ReplayRecorder.h"
#include "../src/replay/ReplayVerifier.h"
#include "../src/runtime/RuntimeState.h"

#include <cstdio>
#include <fstream>
#include <iostream>
#include <string>
#include <vector>

namespace {

dnxr::RuntimeFrame makeFrame(std::uint64_t cycle, float offset) {
    dnxr::RuntimeFrame frame;
    frame.cycleId = cycle;
    frame.timestampMs = static_cast<std::int64_t>(1000 + cycle);
    frame.rawSignal = {0.1f + offset, 0.2f + offset};
    frame.cleanSignal = {0.1f + offset, 0.2f + offset};
    frame.features = {0.15f + offset, 0.1f + offset, 0.2f + offset, 0.05f};
    frame.cognitiveState = "nominal_focus";
    frame.biometrics = {72.0f, 0.12f, 0.91f};
    frame.xrAction = cycle % 2U == 0U ? "xr.navigate" : "xr.interact";
    frame.xrFrame = {0.3f + offset, 0.4f + offset, 0.5f + offset};
    frame.safetyDecision = "allow_render";
    frame.metricsSnapshot.totalCycles = cycle + 1U;
    frame.metricsSnapshot.safetyEventCount = 0U;
    frame.metricsSnapshot.lastLatencyMs = 8.0;
    frame.metricsSnapshot.avgLatencyMs = 8.5;
    frame.metricsSnapshot.jitterMs = 0.2;
    frame.metricsSnapshot.p95LatencyMs = 9.0;
    frame.metricsSnapshot.p99LatencyMs = 9.0;
    frame.metricsSnapshot.latencyHealth = dnxr::RuntimeLatencyHealth::Stable;
    return frame;
}

bool require(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "dnxr_replay_determinism_test: " << message << '\n';
        return false;
    }
    return true;
}

bool parseFails(const std::string& encodedFrame) {
    bool parseOk = true;
    const auto parsed = dnxr::parseRuntimeFrame(encodedFrame, parseOk);
    (void)parsed;
    return !parseOk;
}

} // namespace

int main() {
    dnxr::RuntimeState state(2U);
    const auto frame0 = makeFrame(0U, 0.0f);
    const auto frame1 = makeFrame(1U, 0.1f);
    const auto frame2 = makeFrame(2U, 0.2f);

    state.commitFrame(frame0);
    state.commitFrame(frame1);
    state.commitFrame(frame2);
    if (!require(state.hasFrame(), "runtime state should have a current frame") ||
        !require(state.history().size() == 2U, "runtime history should keep the requested depth") ||
        !require(state.currentFrame().cycleId == 2U, "current frame should be the newest committed frame")) {
        return 1;
    }

    const std::string encoded = dnxr::serializeRuntimeFrame(frame1);
    bool parseOk = false;
    const auto decoded = dnxr::parseRuntimeFrame(encoded, parseOk);
    if (!require(parseOk, "serialized frame should parse successfully") ||
        !require(dnxr::runtimeFrameHash(decoded) == dnxr::runtimeFrameHash(frame1),
                 "serialized frame should round-trip with the same hash")) {
        return 1;
    }

    dnxr::ReplayRecorder recorder;
    recorder.record(frame0);
    recorder.record(frame1);
    recorder.record(frame2);
    const std::string replayPath = "build/dnxr_replay_determinism_test.dnxr_replay";
    if (!require(recorder.saveToFile(replayPath), "recorder should save replay file")) {
        return 1;
    }

    dnxr::ReplayPlayer player;
    if (!require(player.loadFromFile(replayPath), "player should load saved replay file") ||
        !require(player.size() == 3U, "loaded replay should contain three frames") ||
        !require(player.frameHashes().size() == 3U, "loaded replay should contain three frame hashes") ||
        !require(player.hasNext(), "loaded replay should be iterable") ||
        !require(player.next().cycleId == 0U, "first replay frame should have cycle id 0")) {
        std::remove(replayPath.c_str());
        return 1;
    }
    player.reset();
    if (!require(player.next().cycleId == 0U, "reset replay should start at cycle id 0")) {
        std::remove(replayPath.c_str());
        return 1;
    }

    const auto verification = dnxr::ReplayVerifier::verify(recorder.frames(), player.frames());
    if (!require(verification.deterministic, "saved replay should verify as deterministic") ||
        !require(verification.comparedFrames == 3U, "verification should compare all frames") ||
        !require(verification.message == "deterministic_replay_match", "verification should report a match")) {
        std::remove(replayPath.c_str());
        return 1;
    }

    auto mutated = player.frames();
    if (!require(mutated.size() > 1U && !mutated[1].features.empty(),
                 "loaded replay must contain a mutable second frame feature")) {
        std::remove(replayPath.c_str());
        return 1;
    }
    mutated[1].features[0] += 0.01f;
    const auto mismatch = dnxr::ReplayVerifier::verify(recorder.frames(), mutated);
    if (!require(!mismatch.deterministic, "mutated replay should not verify as deterministic") ||
        !require(mismatch.firstMismatchIndex == 1U, "mismatch should identify the second frame") ||
        !require(mismatch.message == "frame_hash_mismatch", "mismatch should report a frame hash failure")) {
        std::remove(replayPath.c_str());
        return 1;
    }

    if (!require(parseFails(
             "9|1009|nan|0.1|0.2|nominal|0.3|xr.look|0.4|allow_render|10|0|1.0|1.0|0.0|1.0|1.0|stable"),
             "parser should reject non-finite vector values") ||
        !require(parseFails(
             "9|1009|0.1|0.1|0.2|nominal|0.3|xr.look|0.4|allow_render|10|0|nan|1.0|0.0|1.0|1.0|stable"),
             "parser should reject non-finite last latency") ||
        !require(parseFails(
             "9|1009|0.1|0.1|0.2|nominal|0.3|xr.look|0.4|allow_render|10|0|1.0|inf|0.0|1.0|1.0|stable"),
             "parser should reject non-finite average latency") ||
        !require(parseFails(
             "9|1009|0.1|0.1|0.2|nominal|0.3|xr.look|0.4|allow_render|10|0|1.0|1.0|-inf|1.0|1.0|stable"),
             "parser should reject non-finite jitter") ||
        !require(parseFails(
             "9|1009|0.1|0.1|0.2|nominal|0.3|xr.look|0.4|allow_render|10|0|1.0|1.0|0.0|nan|1.0|stable"),
             "parser should reject non-finite p95 latency") ||
        !require(parseFails(
             "9|1009|0.1|0.1|0.2|nominal|0.3|xr.look|0.4|allow_render|10|0|1.0|1.0|0.0|1.0|inf|stable"),
             "parser should reject non-finite p99 latency")) {
        std::remove(replayPath.c_str());
        return 1;
    }

    dnxr::RuntimeFrame tamperedFrame = frame0;
    tamperedFrame.rawSignal = {0.1f, 0.2f};
    const std::string tamperedReplayPath = "build/dnxr_replay_nan_tamper_test.dnxr_replay";
    {
        std::ofstream tampered(tamperedReplayPath);
        tampered << "dnxr_replay_v1\n";
        tampered << dnxr::runtimeFrameHash(tamperedFrame) << '|' << dnxr::serializeRuntimeFrame(tamperedFrame) << "\n";
        tampered << "frame_fake|9|1009|nan|0.1|0.2|nominal|0.3|xr.look|0.4|allow_render|10|0|1.0|1.0|0.0|1.0|1.0|stable\n";
    }
    dnxr::ReplayPlayer tamperedPlayer;
    if (!require(!tamperedPlayer.loadFromFile(tamperedReplayPath),
                 "tampered replay containing non-finite data should be rejected")) {
        std::remove(tamperedReplayPath.c_str());
        std::remove(replayPath.c_str());
        return 1;
    }
    std::remove(tamperedReplayPath.c_str());

    std::remove(replayPath.c_str());
    return 0;
}
