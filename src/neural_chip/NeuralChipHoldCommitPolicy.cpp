#include "neural_chip/NeuralChipHoldCommitPolicy.h"

namespace dnxr {

NeuralRealityAnchorVector NeuralChipHoldCommitPolicy::makeSafeHoldRealityAnchor() {
    NeuralRealityAnchorVector anchor{};
    anchor.orientation01 = 1.0f;
    anchor.identityContinuity01 = 1.0f;
    anchor.sessionBoundary01 = 1.0f;
    anchor.externalWorldRecall01 = 1.0f;
    anchor.required = true;
    return anchor;
}

void NeuralChipHoldCommitPolicy::forceHoldSafety(FullDiveChipFrame& frame) {
    frame.motorIntent = NeuralMotorIntentVector{};
    frame.motorIntent.stopIntent01 = 1.0f;
    frame.olfactory = NeuralOlfactoryVector{};
    frame.gustatory = NeuralGustatoryVector{};
    frame.touch.painBlocked01 = 1.0f;
    frame.touch.burnBlocked01 = 1.0f;
    frame.touch.crushBlocked01 = 1.0f;
    frame.touch.shockBlocked01 = 1.0f;
    frame.safety.simulationOnly = true;
    frame.safety.rawSensoryPayloadBlocked = true;
    frame.safety.painMappingBlocked = true;
    frame.safety.patientStopSignalAlive = true;
    frame.safety.emergencyCutoffAvailable = true;
    frame.safety.activeCeiling01 = DNXR_NEURAL_CHIP_ACTIVE_CEILING_01;
    frame.realityAnchor = makeSafeHoldRealityAnchor();
}

FullDiveChipFrame NeuralChipHoldCommitPolicy::makeDecompressionHoldFrame(const FullDiveChipFrame& rejectedFrame, const SafeChipFrameHoldBuffer& holdBuffer, float ramp01) const {
    FullDiveChipFrame frame = holdBuffer.makeDecompressionFrame(rejectedFrame.frameId, rejectedFrame.timestampMs, ramp01);
    forceHoldSafety(frame);
    return frame;
}

FullDiveChipFrame NeuralChipHoldCommitPolicy::makeNeutralHoldFrame(const FullDiveChipFrame& rejectedFrame, const SafeChipFrameHoldBuffer& holdBuffer) const {
    FullDiveChipFrame frame = holdBuffer.makeNeutralHoldFrame(rejectedFrame.frameId, rejectedFrame.timestampMs);
    forceHoldSafety(frame);
    return frame;
}

FullDiveChipFrame NeuralChipHoldCommitPolicy::makeMinimalHardHoldFrame(std::uint64_t frameId, double timestampMs) const {
    FullDiveChipFrame frame{};
    frame.frameId = frameId;
    frame.timestampMs = timestampMs;
    forceHoldSafety(frame);
    frame.safety.aggregateRisk01 = 0.0f;
    return frame;
}

const char* NeuralChipHoldCommitPolicy::fallbackName(NeuralChipHoldFallback fallback) const {
    switch (fallback) {
        case NeuralChipHoldFallback::Decompression: return "decompression hold";
        case NeuralChipHoldFallback::Neutral: return "neutral hold";
        case NeuralChipHoldFallback::MinimalHard: return "minimal hard hold";
        case NeuralChipHoldFallback::BusSafetyOnly: return "bus safety hold only";
    }
    return "unknown hold";
}

} // namespace dnxr
