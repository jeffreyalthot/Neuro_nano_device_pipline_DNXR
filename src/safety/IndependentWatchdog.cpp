#include "IndependentWatchdog.h"

#include <algorithm>
#include <sstream>

namespace dnxr {

IndependentWatchdog::IndependentWatchdog(double heartbeatTimeoutMs, double latencyBudgetMs)
    : heartbeatTimeoutMs_(std::max(1.0, heartbeatTimeoutMs)),
      latencyBudgetMs_(std::max(1.0, latencyBudgetMs)) {
}

void IndependentWatchdog::heartbeat(
    const std::string& subsystem,
    std::chrono::steady_clock::time_point now) {
    heartbeats_[subsystem] = now;
}

WatchdogAssessment IndependentWatchdog::assess(
    double latencyMs,
    bool neuralSignalValid,
    float biometricRisk,
    float driftScore,
    std::chrono::steady_clock::time_point now) const {

    WatchdogAssessment assessment;
    std::ostringstream reason;

    const auto heartbeatTimeout = std::chrono::duration<double, std::milli>(heartbeatTimeoutMs_);
    for (const auto& entry : heartbeats_) {
        if (now - entry.second > heartbeatTimeout) {
            assessment.healthy = false;
            assessment.riskScore = std::max(assessment.riskScore, 0.75f);
            assessment.actions.push_back(WatchdogAction::RealityAnchor);
            reason << "stale_heartbeat:" << entry.first << ";";
        }
    }

    if (latencyMs > latencyBudgetMs_) {
        assessment.healthy = false;
        assessment.riskScore = std::max(assessment.riskScore, static_cast<float>(std::min(1.0, latencyMs / (latencyBudgetMs_ * 2.0))));
        assessment.actions.push_back(WatchdogAction::ReduceIntensiveRendering);
        reason << "latency_over_budget:" << latencyMs << ";";
    }

    if (!neuralSignalValid) {
        assessment.healthy = false;
        assessment.riskScore = std::max(assessment.riskScore, 0.90f);
        assessment.actions.push_back(WatchdogAction::ZeroStimulation);
        reason << "invalid_neural_signal;";
    }

    if (biometricRisk >= 0.85f) {
        assessment.healthy = false;
        assessment.riskScore = std::max(assessment.riskScore, biometricRisk);
        assessment.actions.push_back(WatchdogAction::ZeroStimulation);
        reason << "dangerous_biometrics;";
    }

    if (driftScore >= 0.85f) {
        assessment.healthy = false;
        assessment.riskScore = std::max(assessment.riskScore, driftScore);
        assessment.actions.push_back(WatchdogAction::RealityAnchor);
        reason << "high_cognitive_drift;";
    }

    assessment.requiresEmergencyStop = assessment.riskScore >= 0.90f;
    if (assessment.requiresEmergencyStop) {
        assessment.actions.push_back(WatchdogAction::LockSession);
    }
    assessment.reason = assessment.healthy ? "watchdog_nominal" : reason.str();
    return assessment;
}

void IndependentWatchdog::setHeartbeatTimeoutMs(double timeoutMs) {
    heartbeatTimeoutMs_ = std::max(1.0, timeoutMs);
}

void IndependentWatchdog::setLatencyBudgetMs(double latencyBudgetMs) {
    latencyBudgetMs_ = std::max(1.0, latencyBudgetMs);
}

} // namespace dnxr
