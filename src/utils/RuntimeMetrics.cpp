#include "RuntimeMetrics.h"

#include "Logger.h"

#include <algorithm>
#include <cmath>
#include <string>
#include <vector>

namespace dnxr {
namespace {
constexpr double kThrottleP99BudgetMultiplier = 1.25;
constexpr double kFallbackP99BudgetMultiplier = 1.50;
}

const char* runtimeLatencyHealthToString(RuntimeLatencyHealth health) {
    switch (health) {
    case RuntimeLatencyHealth::Stable:
        return "stable";
    case RuntimeLatencyHealth::Throttle:
        return "throttle";
    case RuntimeLatencyHealth::Fallback:
        return "fallback";
    }
    return "unknown";
}


std::string runtimeMetricsSnapshotToEventPayload(const RuntimeMetricsSnapshot& snapshot) {
    return std::string("metrics:latency_health=") + runtimeLatencyHealthToString(snapshot.latencyHealth) +
           "|total_cycles=" + std::to_string(snapshot.totalCycles) +
           "|safety_events=" + std::to_string(snapshot.safetyEventCount) +
           "|latency_budget_ms=" + std::to_string(snapshot.latencyBudgetMs) +
           "|last_ms=" + std::to_string(snapshot.lastLatencyMs) +
           "|avg_ms=" + std::to_string(snapshot.avgLatencyMs) +
           "|max_ms=" + std::to_string(snapshot.maxLatencyMs) +
           "|jitter_ms=" + std::to_string(snapshot.jitterMs) +
           "|p95_ms=" + std::to_string(snapshot.p95LatencyMs) +
           "|p99_ms=" + std::to_string(snapshot.p99LatencyMs) +
           "|budget_exceeded_total=" + std::to_string(snapshot.latencyBudgetExceededCount) +
           "|budget_exceeded=" + std::to_string(snapshot.windowBudgetExceededCount) +
           "|health_transitions=" + std::to_string(snapshot.latencyHealthTransitionCount) +
           "|non_stable_streak=" + std::to_string(snapshot.consecutiveNonStableCycles);
}

RuntimeMetrics::RuntimeMetrics(double latencyBudgetMs, std::size_t latencyWindowSize)
    : totalCycles_(0),
      totalSafetyEvents_(0),
      latencyBudgetExceededCount_(0),
      latencyHealthTransitionCount_(0),
      consecutiveNonStableCycles_(0),
      currentLatencyHealth_(RuntimeLatencyHealth::Stable),
      latencyBudgetMs_(std::max(0.0, latencyBudgetMs)),
      latencyWindowSize_(std::max<std::size_t>(1, latencyWindowSize)),
      lastLatencyMs_(0.0),
      maxLatencyMs_(0.0),
      meanLatencyMs_(0.0),
      m2Latency_(0.0) {}

void RuntimeMetrics::beginCycle() {
    cycleStart_ = clock::now();
}

void RuntimeMetrics::endCycle(bool safeDecision, bool emergencyActive, const std::string& emergencyReason) {
    const auto elapsed = std::chrono::duration<double, std::milli>(clock::now() - cycleStart_).count();
    recordCycleMeasurement(elapsed, safeDecision, emergencyActive, emergencyReason);
}

void RuntimeMetrics::recordCycleMeasurement(
    double latencyMs,
    bool safeDecision,
    bool emergencyActive,
    const std::string& emergencyReason)
{
    recordLatency(std::max(0.0, latencyMs));
    recordSafetyDecision(safeDecision, emergencyActive, emergencyReason);
}

RuntimeMetricsSnapshot RuntimeMetrics::snapshot() const {
    return RuntimeMetricsSnapshot{
        totalCycles(),
        safetyEventCount(),
        latencyBudgetExceededCount(),
        windowBudgetExceededCount(),
        latencyHealthTransitionCount(),
        consecutiveNonStableCycles(),
        latencyBudgetMs(),
        lastLatencyMs(),
        maxLatencyMs(),
        avgLatencyMs(),
        jitterMs(),
        p95LatencyMs(),
        p99LatencyMs(),
        latencyHealth()
    };
}

RuntimeLatencyHealth RuntimeMetrics::latencyHealth() const {
    return currentLatencyHealth_;
}

RuntimeLatencyHealth RuntimeMetrics::computeLatencyHealth() const {
    if (latencyWindow_.empty() || latencyBudgetMs_ <= 0.0) {
        return latencyWindow_.empty() ? RuntimeLatencyHealth::Stable : RuntimeLatencyHealth::Fallback;
    }

    const double p95 = p95LatencyMs();
    const double p99 = p99LatencyMs();
    if (p99 > latencyBudgetMs_ * kFallbackP99BudgetMultiplier) {
        return RuntimeLatencyHealth::Fallback;
    }
    if (p95 > latencyBudgetMs_ || p99 > latencyBudgetMs_ * kThrottleP99BudgetMultiplier) {
        return RuntimeLatencyHealth::Throttle;
    }
    return RuntimeLatencyHealth::Stable;
}

void RuntimeMetrics::recordLatency(double elapsedMs) {
    lastLatencyMs_ = elapsedMs;
    maxLatencyMs_ = std::max(maxLatencyMs_, elapsedMs);

    ++totalCycles_;
    const double delta = elapsedMs - meanLatencyMs_;
    meanLatencyMs_ += delta / static_cast<double>(totalCycles_);
    const double delta2 = elapsedMs - meanLatencyMs_;
    m2Latency_ += delta * delta2;

    latencyWindow_.push_back(elapsedMs);
    while (latencyWindow_.size() > latencyWindowSize_) {
        latencyWindow_.pop_front();
    }

    if (elapsedMs > latencyBudgetMs_) {
        ++latencyBudgetExceededCount_;
    }

    updateLatencyHealthState();
}

void RuntimeMetrics::updateLatencyHealthState() {
    const RuntimeLatencyHealth nextHealth = computeLatencyHealth();
    if (nextHealth != currentLatencyHealth_) {
        ++latencyHealthTransitionCount_;
        currentLatencyHealth_ = nextHealth;
    }

    if (currentLatencyHealth_ == RuntimeLatencyHealth::Stable) {
        consecutiveNonStableCycles_ = 0;
    } else {
        ++consecutiveNonStableCycles_;
    }
}

void RuntimeMetrics::recordSafetyDecision(
    bool safeDecision,
    bool emergencyActive,
    const std::string& emergencyReason)
{
    if (!safeDecision || emergencyActive) {
        ++totalSafetyEvents_;
        Logger::warn("[RuntimeMetrics] safety_event=" + emergencyReason);
    }
}


void RuntimeMetrics::setLatencyBudgetMs(double latencyBudgetMs) {
    latencyBudgetMs_ = std::max(0.0, latencyBudgetMs);

    const RuntimeLatencyHealth nextHealth = computeLatencyHealth();
    if (nextHealth != currentLatencyHealth_) {
        ++latencyHealthTransitionCount_;
        currentLatencyHealth_ = nextHealth;
    }

    if (currentLatencyHealth_ == RuntimeLatencyHealth::Stable) {
        consecutiveNonStableCycles_ = 0;
    }
}

uint64_t RuntimeMetrics::totalCycles() const { return totalCycles_; }
double RuntimeMetrics::lastLatencyMs() const { return lastLatencyMs_; }
double RuntimeMetrics::maxLatencyMs() const { return maxLatencyMs_; }
double RuntimeMetrics::avgLatencyMs() const { return meanLatencyMs_; }

double RuntimeMetrics::jitterMs() const {
    if (totalCycles_ < 2) return 0.0;
    return std::sqrt(m2Latency_ / static_cast<double>(totalCycles_ - 1));
}

double RuntimeMetrics::p95LatencyMs() const { return percentileLatencyMs(0.95); }
double RuntimeMetrics::p99LatencyMs() const { return percentileLatencyMs(0.99); }
double RuntimeMetrics::latencyBudgetMs() const { return latencyBudgetMs_; }
uint64_t RuntimeMetrics::latencyBudgetExceededCount() const { return latencyBudgetExceededCount_; }
uint64_t RuntimeMetrics::latencyHealthTransitionCount() const { return latencyHealthTransitionCount_; }
uint64_t RuntimeMetrics::consecutiveNonStableCycles() const { return consecutiveNonStableCycles_; }

uint64_t RuntimeMetrics::windowBudgetExceededCount() const {
    return static_cast<uint64_t>(std::count_if(
        latencyWindow_.begin(),
        latencyWindow_.end(),
        [this](double latencyMs) { return latencyMs > latencyBudgetMs_; }));
}

uint64_t RuntimeMetrics::safetyEventCount() const { return totalSafetyEvents_; }

double RuntimeMetrics::percentileLatencyMs(double percentile) const {
    if (latencyWindow_.empty()) {
        return 0.0;
    }

    std::vector<double> samples(latencyWindow_.begin(), latencyWindow_.end());
    std::sort(samples.begin(), samples.end());

    const double boundedPercentile = std::clamp(percentile, 0.0, 1.0);
    if (boundedPercentile <= 0.0) {
        return samples.front();
    }

    const auto index = static_cast<std::size_t>(
        std::ceil(boundedPercentile * static_cast<double>(samples.size())) - 1.0);
    return samples[std::min(index, samples.size() - 1)];
}

} // namespace dnxr
