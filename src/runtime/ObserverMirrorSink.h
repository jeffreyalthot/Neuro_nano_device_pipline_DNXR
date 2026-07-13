#pragma once

#include <atomic>
#include <mutex>
#include <string>
#include <vector>

namespace dnxr {

class XRRenderer;

class ObserverMirrorSink {
public:
    ObserverMirrorSink() = default;

    void setEnabled(bool enabled) noexcept { enabled_.store(enabled); }
    bool enabled() const noexcept { return enabled_.load(); }
    void attachRenderer(XRRenderer* renderer) noexcept { renderer_ = renderer; }

    bool publishImmutableCopy(const std::vector<float>& neuralSensoryFrame) noexcept;
    std::vector<float> lastFrameCopy() const;
    std::string lastFailureReason() const;
    std::uint64_t publishedCount() const noexcept { return publishedCount_.load(); }
    std::uint64_t failedCount() const noexcept { return failedCount_.load(); }

private:
    std::atomic<bool> enabled_{false};
    XRRenderer* renderer_ = nullptr; // observer-only, never consulted by neural safety/dispatch
    mutable std::mutex mutex_;
    std::vector<float> lastFrame_;
    std::string lastFailure_;
    std::atomic<std::uint64_t> publishedCount_{0};
    std::atomic<std::uint64_t> failedCount_{0};
};

} // namespace dnxr
