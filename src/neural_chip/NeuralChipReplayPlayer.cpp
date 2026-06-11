#include "neural_chip/NeuralChipReplayPlayer.h"
#include "neural_chip/NeuralChipReplayRecorder.h"
#include "config/JsonParser.h"
#include "config/JsonValue.h"

#include <fstream>
#include <string>

namespace dnxr {
namespace {
bool forbidden(const std::string& s) {
    return s.find("payload") != std::string::npos || s.find("rawSensory") != std::string::npos ||
           s.find(std::string("human") + "-ready") != std::string::npos || s.find(std::string("clinical") + "-ready") != std::string::npos;
}

bool hasType(const JsonValue& root, const char* key, JsonValue::Type type) {
    const auto* value = root.find(key);
    return value && value->type() == type;
}
} // namespace

NeuralChipReplayPlaybackReport NeuralChipReplayPlayer::validateJsonl(const std::string& path) const {
    std::ifstream in(path);
    if (!in) return {false, "replay file unavailable", 0};
    std::string line;
    std::uint64_t lastFrameId = 0;
    std::size_t count = 0;
    while (std::getline(in, line)) {
        if (line.empty()) continue;
        if (forbidden(line)) return {false, "replay contains forbidden payload or readiness claim", count};
        JsonValue root;
        std::string error;
        if (!JsonParser::tryParse(line, root, &error) || !root.isObject()) return {false, "invalid replay json", count};
        if (root.getString("schemaVersion", "") != DNXR_REPLAY_SCHEMA_VERSION) return {false, "unknown replay schema", count};
        for (const char* key : {"frameId", "timestampMs", "pipelineState", "pipelinePathKind", "stepOutcome", "noOp", "busSubmitted", "candidateAccepted", "normalFrameSubmitted", "holdFrameSubmitted", "safetyHold", "traceCoverageComplete", "traceCoverageMissingStage", "sessionDigestHash", "sessionDigestFrameCount", "hasCandidateFrame", "hasSubmittedFrame"}) {
            if (!root.has(key)) return {false, "replay line missing required v2 field", count};
        }
        if (!hasType(root, "pipelineState", JsonValue::Type::String) || !hasType(root, "pipelinePathKind", JsonValue::Type::String) || !hasType(root, "stepOutcome", JsonValue::Type::String)) return {false, "replay line has invalid string field", count};
        if (!hasType(root, "noOp", JsonValue::Type::Bool) || !hasType(root, "busSubmitted", JsonValue::Type::Bool) || !hasType(root, "traceCoverageComplete", JsonValue::Type::Bool)) return {false, "replay line has invalid boolean field", count};
        const auto frameId = root.getUInt64("frameId", 0);
        if (frameId == 0) return {false, "invalid frameId", count};
        if (count > 0 && frameId <= lastFrameId) return {false, "non-monotone replay frameId", count};
        const std::string outcome = root.getString("stepOutcome", "");
        const bool noOp = root.getBool("noOp", false);
        const bool busSubmitted = root.getBool("busSubmitted", false);
        if ((outcome == "WaitingForAck" || outcome == "Shutdown") && (!noOp || busSubmitted || root.getBool("hasCandidateFrame", true) || root.getBool("hasSubmittedFrame", true))) return {false, "no-op outcome has submitted frame fields", count};
        if (outcome == "NormalAccepted" && (!busSubmitted || !root.getBool("normalFrameSubmitted", false))) return {false, "normal accepted missing bus submission", count};
        lastFrameId = frameId;
        ++count;
    }
    return {count > 0, count > 0 ? "valid" : "empty replay", count};
}

} // namespace dnxr
