#include "neural_chip/FrameLifecycleGuard.h"

#include <cmath>

namespace dnxr {

FrameLifecycleGuard::FrameLifecycleGuard(FrameLifecyclePolicy policy) : policy_(policy) {}

FrameLifecycleValidation FrameLifecycleGuard::preview(const FullDiveChipFrame& frame) const {
    if (!std::isfinite(frame.timestampMs)) {
        return {false, "timestamp is non-finite", true};
    }
    if (frame.frameId == 0) {
        return {false, "frameId is invalid", true};
    }
    if (frame.timestampMs < 0.0) {
        return {false, "timestamp is negative", true};
    }
    if (hasLast_ && frame.frameId <= lastFrameId_) {
        return {false, "frame replay or non-monotone frameId", true};
    }
    if (hasLast_ && frame.timestampMs <= lastTimestampMs_) {
        return {false, "non-monotone timestamp", true};
    }
    if (policy_.nowMs > 0.0) {
        if (frame.timestampMs > policy_.nowMs + policy_.maxFutureSkewMs) {
            return {false, "timestamp too far in future", true};
        }
        if (frame.timestampMs < policy_.nowMs - policy_.maxFrameAgeMs) {
            return {false, "frame too old", true};
        }
    }
    if (hasLast_ && frame.timestampMs - lastTimestampMs_ > policy_.maxFrameGapMs) {
        return {false, "frame deadline gap exceeded", true};
    }
    return {true, "accepted", false};
}

void FrameLifecycleGuard::commitAccepted(const FullDiveChipFrame& frame) {
    hasLast_ = true;
    lastFrameId_ = frame.frameId;
    lastTimestampMs_ = frame.timestampMs;
}

void FrameLifecycleGuard::rollbackPreview() {}

FrameLifecycleValidation FrameLifecycleGuard::validate(const FullDiveChipFrame& frame) {
    const auto result = preview(frame);
    if (result.accepted) commitAccepted(frame);
    return result;
}

void FrameLifecycleGuard::reset() {
    hasLast_ = false;
    lastFrameId_ = 0;
    lastTimestampMs_ = 0.0;
}

void FrameLifecycleGuard::setPolicy(FrameLifecyclePolicy policy) {
    policy_ = policy;
}

} // namespace dnxr
