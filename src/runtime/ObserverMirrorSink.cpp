#include "ObserverMirrorSink.h"

#if DNXR_SIMULATION_ONLY
#include "../rendering/XRRenderer.h"
#endif

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
#if DNXR_SIMULATION_ONLY
    if (!renderer_) {
        std::lock_guard<std::mutex> lock(mutex_);
        lastFailure_ = "observer_mirror_renderer_unavailable";
        ++failedCount_;
        return false;
    }
    renderer_->present(immutableCopy);
    ++publishedCount_;
    return true;
#else
    std::lock_guard<std::mutex> lock(mutex_);
    lastFailure_ = "observer_mirror_renderer_excluded_from_real_runtime";
    ++failedCount_;
    return false;
#endif
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
