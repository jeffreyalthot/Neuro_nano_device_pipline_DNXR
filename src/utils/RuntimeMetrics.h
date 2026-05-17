#pragma once

#include <chrono>
#include <cstdint>
#include <string>

namespace dnxr {

class RuntimeMetrics {
public:
    RuntimeMetrics();

    void beginCycle();
    void endCycle(bool safeDecision, bool emergencyActive, const std::string& emergencyReason);

    uint64_t totalCycles() const;
    double lastLatencyMs() const;
    double maxLatencyMs() const;
    double avgLatencyMs() const;
    double jitterMs() const;
    uint64_t safetyEventCount() const;

private:
    using clock = std::chrono::high_resolution_clock;
    clock::time_point cycleStart_;

    uint64_t totalCycles_;
    uint64_t totalSafetyEvents_;
    double lastLatencyMs_;
    double maxLatencyMs_;
    double meanLatencyMs_;
    double m2Latency_;
};

} // namespace dnxr
