#pragma once

#include "neural_chip/FrameLifecycleGuard.h"
#include "neural_chip/NeuralChipBus.h"
#include "neural_chip/NeuralChipHoldCommitPolicy.h"
#include "neural_chip/SafeChipFrameHoldBuffer.h"

namespace dnxr {

struct FrameCommitResult {
    bool accepted = false;
    bool safetyHold = false;
    const char* reason = "";
    const char* holdFallback = "";
    FullDiveChipFrame submittedFrame{};
};

class NeuralChipFrameCommitter {
public:
    NeuralChipFrameCommitter(NeuralChipBus& bus, FrameLifecycleGuard& lifecycle, SafeChipFrameHoldBuffer& holdBuffer);

    FrameCommitResult commitCandidate(const FullDiveChipFrame& frame);
    FrameCommitResult commitRejectedWithHold(const FullDiveChipFrame& rejectedFrame, const char* reason, float decompressionRamp01);

private:
    NeuralChipBus& bus_;
    FrameLifecycleGuard& lifecycle_;
    SafeChipFrameHoldBuffer& holdBuffer_;
    NeuralChipHoldCommitPolicy holdPolicy_{};
};

} // namespace dnxr
