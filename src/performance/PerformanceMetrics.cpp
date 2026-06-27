#include "PerformanceMetrics.h"

#include <sstream>

namespace dnxr {

double PerformanceMetrics::averageMs() const noexcept {
    return samples == 0 ? 0.0 : totalMs / static_cast<double>(samples);
}

void PerformanceMetrics::observe(double latencyMs) noexcept {
    lastMs = latencyMs;
    if (samples == 0 || latencyMs < minMs) minMs = latencyMs;
    if (samples == 0 || latencyMs > maxMs) maxMs = latencyMs;
    totalMs += latencyMs;
    ++samples;
}

std::string PerformanceMetrics::summary() const {
    std::ostringstream out;
    out << "samples=" << samples << " last_ms=" << lastMs << " avg_ms=" << averageMs()
        << " min_ms=" << minMs << " max_ms=" << maxMs;
    return out.str();
}

} // namespace dnxr
