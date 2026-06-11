#include "neural_chip/NeuralChipFrameCommitter.h"
#include "neural_chip/NeuralVectorValidator.h"

namespace dnxr {

NeuralChipFrameCommitter::NeuralChipFrameCommitter(NeuralChipBus& bus, FrameLifecycleGuard& lifecycle, SafeChipFrameHoldBuffer& holdBuffer)
    : bus_(bus), lifecycle_(lifecycle), holdBuffer_(holdBuffer) {}

FrameCommitResult NeuralChipFrameCommitter::commitCandidate(const FullDiveChipFrame& frame) {
    const auto lifecycle = lifecycle_.validate(frame);
    if (!lifecycle.accepted) {
        return commitRejectedWithHold(frame, lifecycle.reason, 0.0f);
    }

    const auto validation = NeuralVectorValidator::validate(frame);
    if (!validation.accepted) {
        return commitRejectedWithHold(frame, validation.reason, 0.25f);
    }

    if (!bus_.submitSensoryFrame(frame)) {
        bus_.submitSafetyHold({frame.frameId, true, "neural chip bus rejected candidate"});
        return {false, true, "neural chip bus rejected candidate", "bus safety hold only", frame};
    }

    holdBuffer_.rememberAccepted(frame);
    return {true, false, "accepted", "", frame};
}

FrameCommitResult NeuralChipFrameCommitter::commitRejectedWithHold(const FullDiveChipFrame& rejectedFrame, const char* reason, float decompressionRamp01) {
    const char* safeReason = reason ? reason : "safety hold";
    struct Candidate { NeuralChipHoldFallback fallback; FullDiveChipFrame frame; };
    Candidate candidates[] = {
        {NeuralChipHoldFallback::Decompression, holdPolicy_.makeDecompressionHoldFrame(rejectedFrame, holdBuffer_, decompressionRamp01)},
        {NeuralChipHoldFallback::Neutral, holdPolicy_.makeNeutralHoldFrame(rejectedFrame, holdBuffer_)},
        {NeuralChipHoldFallback::MinimalHard, holdPolicy_.makeMinimalHardHoldFrame(rejectedFrame.frameId, rejectedFrame.timestampMs)},
    };

    for (const auto& candidate : candidates) {
        const auto validation = NeuralVectorValidator::validate(candidate.frame);
        if (!validation.accepted) {
            continue;
        }
        if (bus_.submitSensoryFrame(candidate.frame)) {
            bus_.submitSafetyHold({rejectedFrame.frameId, true, safeReason});
            return {false, true, safeReason, holdPolicy_.fallbackName(candidate.fallback), candidate.frame};
        }
    }

    bus_.submitSafetyHold({rejectedFrame.frameId, true, safeReason});
    return {false, true, "hold frame rejected; bus safety hold active", holdPolicy_.fallbackName(NeuralChipHoldFallback::BusSafetyOnly),
            holdPolicy_.makeMinimalHardHoldFrame(rejectedFrame.frameId, rejectedFrame.timestampMs)};
}

} // namespace dnxr
