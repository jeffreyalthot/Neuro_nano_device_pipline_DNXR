#include "../src/replay/ReplayPlayer.h"
#include "../src/replay/ReplayRecorder.h"
#include "../src/replay/ReplayVerifier.h"
#include "../src/runtime/RuntimeState.h"

#include <cassert>
#include <cstdio>
#include <fstream>
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

} // namespace

int main() {
    dnxr::RuntimeState state(2U);
    const auto frame0 = makeFrame(0U, 0.0f);
    const auto frame1 = makeFrame(1U, 0.1f);
    const auto frame2 = makeFrame(2U, 0.2f);

    state.commitFrame(frame0);
    state.commitFrame(frame1);
    state.commitFrame(frame2);
    assert(state.hasFrame());
    assert(state.history().size() == 2U);
    assert(state.currentFrame().cycleId == 2U);

    const std::string encoded = dnxr::serializeRuntimeFrame(frame1);
    bool parseOk = false;
    const auto decoded = dnxr::parseRuntimeFrame(encoded, parseOk);
    assert(parseOk);
    assert(dnxr::runtimeFrameHash(decoded) == dnxr::runtimeFrameHash(frame1));

    dnxr::ReplayRecorder recorder;
    recorder.record(frame0);
    recorder.record(frame1);
    recorder.record(frame2);
    const std::string replayPath = "build/dnxr_replay_determinism_test.dnxr_replay";
    assert(recorder.saveToFile(replayPath));

    dnxr::ReplayPlayer player;
    assert(player.loadFromFile(replayPath));
    assert(player.size() == 3U);
    assert(player.frameHashes().size() == 3U);
    assert(player.hasNext());
    assert(player.next().cycleId == 0U);
    player.reset();
    assert(player.next().cycleId == 0U);

    const auto verification = dnxr::ReplayVerifier::verify(recorder.frames(), player.frames());
    assert(verification.deterministic);
    assert(verification.comparedFrames == 3U);
    assert(verification.message == "deterministic_replay_match");

    auto mutated = player.frames();
    mutated[1].features[0] += 0.01f;
    const auto mismatch = dnxr::ReplayVerifier::verify(recorder.frames(), mutated);
    assert(!mismatch.deterministic);
    assert(mismatch.firstMismatchIndex == 1U);
    assert(mismatch.message == "frame_hash_mismatch");


    bool invalidParseOk = true;
    const auto invalidNaNFrame = dnxr::parseRuntimeFrame(
        "9|1009|nan|0.1|0.2|nominal|0.3|xr.look|0.4|allow_render|10|0|1.0|1.0|0.0|1.0|1.0|stable",
        invalidParseOk);
    (void)invalidNaNFrame;
    assert(!invalidParseOk);

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
    assert(!tamperedPlayer.loadFromFile(tamperedReplayPath));
    std::remove(tamperedReplayPath.c_str());

    std::remove(replayPath.c_str());
    return 0;
}
