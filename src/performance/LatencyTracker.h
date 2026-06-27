#pragma once

#include "PerformanceMetrics.h"
#include <chrono>

namespace dnxr {

class LatencyTracker {
public:
    void begin() noexcept;
    double end() noexcept;
    const PerformanceMetrics& metrics() const noexcept { return metrics_; }
private:
    std::chrono::steady_clock::time_point start_{};
    PerformanceMetrics metrics_{};
};

} // namespace dnxr
