#pragma once

#include <chrono>
#include <cstddef>
#include <cstdint>
#include <deque>
#include <string>

namespace dnxr {

enum class RuntimeLatencyHealth {
    Stable,
    Throttle,
    Fallback
};

const char* runtimeLatencyHealthToString(RuntimeLatencyHealth health);

struct RuntimeMetricsSnapshot {
    uint64_t totalCycles{0};
    uint64_t safetyEventCount{0};
    uint64_t latencyBudgetExceededCount{0};
    uint64_t windowBudgetExceededCount{0};
    uint64_t latencyHealthTransitionCount{0};
    uint64_t consecutiveNonStableCycles{0};
    double latencyBudgetMs{20.0};
    double lastLatencyMs{0.0};
    double maxLatencyMs{0.0};
    double avgLatencyMs{0.0};
    double jitterMs{0.0};
    double p95LatencyMs{0.0};
    double p99LatencyMs{0.0};
    RuntimeLatencyHealth latencyHealth{RuntimeLatencyHealth::Stable};
};

std::string runtimeMetricsSnapshotToEventPayload(const RuntimeMetricsSnapshot& snapshot);

class RuntimeMetrics {
public:
    explicit RuntimeMetrics(double latencyBudgetMs = 20.0, std::size_t latencyWindowSize = 256);

    void beginCycle();
    void endCycle(bool safeDecision, bool emergencyActive, const std::string& emergencyReason);

    void recordCycleMeasurement(
        double latencyMs,
        bool safeDecision = true,
        bool emergencyActive = false,
        const std::string& emergencyReason = "manual_runtime_sample");

    RuntimeMetricsSnapshot snapshot() const;
    RuntimeLatencyHealth latencyHealth() const;
    void setLatencyBudgetMs(double latencyBudgetMs);

    uint64_t totalCycles() const;
    double lastLatencyMs() const;
    double maxLatencyMs() const;
    double avgLatencyMs() const;
    double jitterMs() const;
    double p95LatencyMs() const;
    double p99LatencyMs() const;
    double latencyBudgetMs() const;
    uint64_t latencyBudgetExceededCount() const;
    uint64_t windowBudgetExceededCount() const;
    uint64_t latencyHealthTransitionCount() const;
    uint64_t consecutiveNonStableCycles() const;
    uint64_t safetyEventCount() const;

private:
    using clock = std::chrono::steady_clock;

    double percentileLatencyMs(double percentile) const;
    RuntimeLatencyHealth computeLatencyHealth() const;
    void recordLatency(double elapsedMs);
    void updateLatencyHealthState();
    void recordSafetyDecision(bool safeDecision, bool emergencyActive, const std::string& emergencyReason);

    clock::time_point cycleStart_;

    uint64_t totalCycles_;
    uint64_t totalSafetyEvents_;
    uint64_t latencyBudgetExceededCount_;
    uint64_t latencyHealthTransitionCount_;
    uint64_t consecutiveNonStableCycles_;
    RuntimeLatencyHealth currentLatencyHealth_;
    double latencyBudgetMs_;
    std::size_t latencyWindowSize_;
    std::deque<double> latencyWindow_;
    double lastLatencyMs_;
    double maxLatencyMs_;
    double meanLatencyMs_;
    double m2Latency_;
};

} // namespace dnxr
