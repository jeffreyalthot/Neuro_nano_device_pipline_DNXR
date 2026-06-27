#pragma once

#include <cstdint>
#include <string>

namespace dnxr {

struct PerformanceMetrics {
    std::uint64_t samples = 0;
    double lastMs = 0.0;
    double minMs = 0.0;
    double maxMs = 0.0;
    double totalMs = 0.0;
    double averageMs() const noexcept;
    void observe(double latencyMs) noexcept;
    std::string summary() const;
};

} // namespace dnxr
