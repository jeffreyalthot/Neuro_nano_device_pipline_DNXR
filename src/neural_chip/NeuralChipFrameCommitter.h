#pragma once

#include "neural_chip/FrameLifecycleGuard.h"
#include "neural_chip/NeuralChipBus.h"
#include "neural_chip/HoldFrameSequencer.h"
#include "neural_chip/NeuralChipHoldCommitPolicy.h"
#include "neural_chip/SafeChipFrameHoldBuffer.h"

#include <string>

namespace dnxr {

struct FrameCommitResult {
    bool candidateAccepted = false;
    bool accepted = false;
    bool busSubmitted = false;
    bool normalFrameSubmitted = false;
    bool holdFrameSubmitted = false;
    bool safetyHold = false;
    bool lifecycleCommitted = false;
    std::string reason;
    std::string holdFallback;
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
    HoldFrameSequencer holdSequencer_{};
};

} // namespace dnxr
