#include "RuntimeMetrics.h"

#include "Logger.h"

#include <algorithm>
#include <cmath>

namespace dnxr {

RuntimeMetrics::RuntimeMetrics()
    : totalCycles_(0),
      totalSafetyEvents_(0),
      lastLatencyMs_(0.0),
      maxLatencyMs_(0.0),
      meanLatencyMs_(0.0),
      m2Latency_(0.0) {}

void RuntimeMetrics::beginCycle() {
    cycleStart_ = clock::now();
}

void RuntimeMetrics::endCycle(bool safeDecision, bool emergencyActive, const std::string& emergencyReason) {
    const auto elapsed = std::chrono::duration<double, std::milli>(clock::now() - cycleStart_).count();
    lastLatencyMs_ = elapsed;
    maxLatencyMs_ = std::max(maxLatencyMs_, elapsed);

    ++totalCycles_;
    const double delta = elapsed - meanLatencyMs_;
    meanLatencyMs_ += delta / static_cast<double>(totalCycles_);
    const double delta2 = elapsed - meanLatencyMs_;
    m2Latency_ += delta * delta2;

    if (!safeDecision || emergencyActive) {
        ++totalSafetyEvents_;
        Logger::warn("[RuntimeMetrics] safety_event=" + emergencyReason);
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

uint64_t RuntimeMetrics::safetyEventCount() const { return totalSafetyEvents_; }

} // namespace dnxr
