#include "ObserverMirrorSink.h"

#include "../rendering/XRRenderer.h"

namespace dnxr {

bool ObserverMirrorSink::publishImmutableCopy(const std::vector<float>& neuralSensoryFrame) noexcept {
    if (!enabled_.load()) {
        return true;
    }
    const std::vector<float> immutableCopy = neuralSensoryFrame;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        lastFrame_ = immutableCopy;
        lastFailure_.clear();
    }
    if (!renderer_) {
        std::lock_guard<std::mutex> lock(mutex_);
        lastFailure_ = "observer_mirror_renderer_unavailable";
        ++failedCount_;
        return false;
    }
    renderer_->present(immutableCopy);
    ++publishedCount_;
    return true;
}

std::vector<float> ObserverMirrorSink::lastFrameCopy() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return lastFrame_;
}

std::string ObserverMirrorSink::lastFailureReason() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return lastFailure_;
}

} // namespace dnxr
