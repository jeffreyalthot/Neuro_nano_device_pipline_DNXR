#include "NeuralIntentProtocol.h"

#include <algorithm>
#include <array>
#include <cmath>

namespace dnxr {

namespace {

struct CandidateScore {
    NeuralIntentAction action;
    float score;
};

constexpr float kAmbiguityMargin = 0.05f;
constexpr float kFallbackConfidenceCeiling = 0.44f;

} // namespace

NeuralIntentVector NeuralIntentProtocol::decode(const std::vector<float>& neuralVector, float baseConfidence) const {
    return decodeWithSchema(neuralVector, baseConfidence, kLatestSchema);
}

NeuralIntentVector NeuralIntentProtocol::decodeWithSchema(
    const std::vector<float>& neuralVector,
    float baseConfidence,
    const std::string& schemaVersion) const
{
    NeuralIntentVector out;
    out.schemaVersion = isCompatibleSchema(schemaVersion) ? schemaVersion : kLatestSchema;
    out.actionToken = actionToken(out.action);
    if (neuralVector.empty()) return out;

    const auto n = neuralVector.size();
    auto s = [&](std::size_t i) { return std::abs(neuralVector[i % n]); };

    const float grabScore      = s(0) + s(1);
    const float rotateScore    = s(4) + s(5);
    const float moveScore      = s(2) + s(3);
    const float interruptScore = s(6);
    const float cancelScore    = s(7);
    const float lookScore      = s(8) + s(9);
    const float composeScore   = (out.schemaVersion == kSchemaV11)
                                   ? (s(10) + s(11) + std::min(grabScore, rotateScore) * 0.25f)
                                   : 0.0f;

    std::array<CandidateScore, 7> candidates{{
        {NeuralIntentAction::Grab, grabScore},
        {NeuralIntentAction::Rotate, rotateScore},
        {NeuralIntentAction::Move, moveScore},
        {NeuralIntentAction::Interrupt, interruptScore},
        {NeuralIntentAction::Cancel, cancelScore},
        {NeuralIntentAction::Look, lookScore},
        {NeuralIntentAction::Compose, composeScore},
    }};

    std::sort(candidates.begin(), candidates.end(), [](const CandidateScore& lhs, const CandidateScore& rhs) {
        return lhs.score > rhs.score;
    });

    const float best = candidates[0].score;
    const float second = candidates[1].score;
    out.ambiguityMargin = std::clamp(best - second, 0.0f, 1.0f);
    out.ambiguityDetected = out.ambiguityMargin < kAmbiguityMargin;

    out.action = out.ambiguityDetected ? NeuralIntentAction::None : candidates[0].action;
    out.actionToken = actionToken(out.action);
    out.intensity = std::clamp(best * 0.1f, 0.0f, 1.0f);
    out.confidence = std::clamp(baseConfidence * 0.7f + out.intensity * 0.3f, 0.0f, 1.0f);
    if (out.ambiguityDetected) {
        out.fallbackRecommended = true;
        out.confidence = std::min(out.confidence, kFallbackConfidenceCeiling);
    }

    out.composable = (out.action == NeuralIntentAction::Move ||
                      out.action == NeuralIntentAction::Look ||
                      out.action == NeuralIntentAction::Rotate ||
                      out.action == NeuralIntentAction::Compose);
    return out;
}

bool NeuralIntentProtocol::isCompatibleSchema(const std::string& schemaVersion) const {
    return schemaVersion == kSchemaV1 || schemaVersion == kSchemaV11;
}

std::string NeuralIntentProtocol::actionToken(NeuralIntentAction action) const {
    switch (action) {
        case NeuralIntentAction::Grab:      return "xr.grab";
        case NeuralIntentAction::Rotate:    return "xr.rotate";
        case NeuralIntentAction::Interrupt: return "xr.interrupt";
        case NeuralIntentAction::Cancel:    return "xr.cancel";
        case NeuralIntentAction::Move:      return "xr.move";
        case NeuralIntentAction::Look:      return "xr.look";
        case NeuralIntentAction::Compose:   return "xr.compose";
        case NeuralIntentAction::None:      return "none";
    }
    return "none";
}

} // namespace dnxr
