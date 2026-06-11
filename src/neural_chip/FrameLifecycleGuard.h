#pragma once

#include "neural_chip/NeuralChipFrame.h"

#include <cstdint>

namespace dnxr {

struct FrameLifecycleValidation {
    bool accepted = false;
    const char* reason = "";
    bool forceSafetyHold = false;
};

struct FrameLifecyclePolicy {
    double nowMs = 0.0;
    double maxFutureSkewMs = 50.0;
    double maxFrameAgeMs = 250.0;
    double maxFrameGapMs = 100.0;
};

class FrameLifecycleGuard {
public:
    explicit FrameLifecycleGuard(FrameLifecyclePolicy policy = {});
    FrameLifecycleValidation preview(const FullDiveChipFrame& frame) const;
    void commitAccepted(const FullDiveChipFrame& frame);
    void rollbackPreview();
    FrameLifecycleValidation validate(const FullDiveChipFrame& frame);
    void reset();
    void setPolicy(FrameLifecyclePolicy policy);

private:
    FrameLifecyclePolicy policy_{};
    bool hasLast_ = false;
    std::uint64_t lastFrameId_ = 0;
    double lastTimestampMs_ = 0.0;
};

} // namespace dnxr
