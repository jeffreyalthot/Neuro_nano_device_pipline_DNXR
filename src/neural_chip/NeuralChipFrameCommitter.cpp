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
        return {false, true, "neural chip bus rejected candidate"};
    }

    holdBuffer_.rememberAccepted(frame);
    return {true, false, "accepted"};
}

FrameCommitResult NeuralChipFrameCommitter::commitRejectedWithHold(const FullDiveChipFrame& rejectedFrame, const char* reason, float decompressionRamp01) {
    const char* safeReason = reason ? reason : "safety hold";
    FullDiveChipFrame hold = decompressionRamp01 > 0.0f
        ? holdBuffer_.makeDecompressionFrame(rejectedFrame.frameId, rejectedFrame.timestampMs, decompressionRamp01)
        : holdBuffer_.makeNeutralHoldFrame(rejectedFrame.frameId, rejectedFrame.timestampMs);
    bus_.submitSensoryFrame(hold);
    bus_.submitSafetyHold({rejectedFrame.frameId, true, safeReason});
    return {false, true, safeReason};
}

} // namespace dnxr
