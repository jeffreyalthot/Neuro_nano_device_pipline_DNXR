#include "neural_chip/NeuralChipFrameCommitter.h"
#include "neural_chip/NeuralVectorValidator.h"

namespace dnxr {

NeuralChipFrameCommitter::NeuralChipFrameCommitter(NeuralChipBus& bus, FrameLifecycleGuard& lifecycle, SafeChipFrameHoldBuffer& holdBuffer)
    : bus_(bus), lifecycle_(lifecycle), holdBuffer_(holdBuffer) {}

FrameCommitResult NeuralChipFrameCommitter::commitCandidate(const FullDiveChipFrame& frame) {
    const auto lifecycle = lifecycle_.preview(frame);
    if (!lifecycle.accepted) {
        return commitRejectedWithHold(frame, lifecycle.reason, 0.0f);
    }

    const auto validation = NeuralVectorValidator::validate(frame);
    if (!validation.accepted) {
        return commitRejectedWithHold(frame, validation.reason, 0.25f);
    }

    if (!bus_.submitSensoryFrame(frame)) {
        bus_.submitSafetyHold({frame.frameId, true, "neural chip bus rejected candidate"});
        return {false, false, true, false, false, true, false, "neural chip bus rejected candidate", "bus safety hold only", frame};
    }

    lifecycle_.commitAccepted(frame);
    holdBuffer_.rememberAccepted(frame);
    holdSequencer_.observeCommitted(frame.frameId, frame.timestampMs);
    return {true, true, true, true, false, false, true, "accepted", "", frame};
}

FrameCommitResult NeuralChipFrameCommitter::commitRejectedWithHold(const FullDiveChipFrame& rejectedFrame, const char* reason, float decompressionRamp01) {
    const char* safeReason = reason ? reason : "safety hold";
    struct Candidate { NeuralChipHoldFallback fallback; FullDiveChipFrame frame; const char* kind; };
    Candidate candidates[] = {
        {NeuralChipHoldFallback::Decompression, holdPolicy_.makeDecompressionHoldFrame(rejectedFrame, holdBuffer_, decompressionRamp01), "decompression hold"},
        {NeuralChipHoldFallback::Neutral, holdPolicy_.makeNeutralHoldFrame(rejectedFrame, holdBuffer_), "neutral hold"},
        {NeuralChipHoldFallback::MinimalHard, holdPolicy_.makeMinimalHardHoldFrame(rejectedFrame.frameId, rejectedFrame.timestampMs), "minimal hard hold"},
    };

    for (auto& candidate : candidates) {
        const auto token = holdSequencer_.nextHold(rejectedFrame.timestampMs, candidate.kind);
        candidate.frame.frameId = token.frameId;
        candidate.frame.timestampMs = token.timestampMs;
        const auto lifecycle = lifecycle_.preview(candidate.frame);
        if (!lifecycle.accepted) {
            continue;
        }
        const auto validation = NeuralVectorValidator::validate(candidate.frame);
        if (!validation.accepted) {
            continue;
        }
        if (bus_.submitSensoryFrame(candidate.frame)) {
            lifecycle_.commitAccepted(candidate.frame);
            holdBuffer_.rememberAccepted(candidate.frame);
            holdSequencer_.observeCommitted(candidate.frame.frameId, candidate.frame.timestampMs);
            bus_.submitSafetyHold({candidate.frame.frameId, true, safeReason});
            return {false, false, true, false, true, true, true, safeReason, holdPolicy_.fallbackName(candidate.fallback), candidate.frame};
        }
    }

    const auto token = holdSequencer_.nextHold(rejectedFrame.timestampMs, "bus safety hold only");
    bus_.submitSafetyHold({token.frameId, true, safeReason});
    FullDiveChipFrame minimal = holdPolicy_.makeMinimalHardHoldFrame(token.frameId, token.timestampMs);
    return {false, false, true, false, false, true, false, "hold frame rejected; bus safety hold active", holdPolicy_.fallbackName(NeuralChipHoldFallback::BusSafetyOnly), minimal};
}

} // namespace dnxr
