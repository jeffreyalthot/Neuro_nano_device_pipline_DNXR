#include "LatencyTracker.h"

namespace dnxr {

void LatencyTracker::begin() noexcept { start_ = std::chrono::steady_clock::now(); }

double LatencyTracker::end() noexcept {
    const auto elapsed = std::chrono::duration<double, std::milli>(std::chrono::steady_clock::now() - start_).count();
    metrics_.observe(elapsed);
    return elapsed;
}

} // namespace dnxr
