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
    std::lock_guard<std::mutex> lock(mutex_);
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

    std::vector<std::string> requiredSubsystems;
    std::map<std::string, std::chrono::steady_clock::time_point> heartbeats;
    double heartbeatTimeoutMs = 0.0;
    double latencyBudgetMs = 0.0;
    {
        std::lock_guard<std::mutex> lock(mutex_);
        requiredSubsystems = requiredSubsystems_;
        heartbeats = heartbeats_;
        heartbeatTimeoutMs = heartbeatTimeoutMs_;
        latencyBudgetMs = latencyBudgetMs_;
    }

    const auto heartbeatTimeout = std::chrono::duration<double, std::milli>(heartbeatTimeoutMs);
    for (const auto& subsystem : requiredSubsystems) {
        if (heartbeats.find(subsystem) == heartbeats.end()) {
            assessment.healthy = false;
            assessment.riskScore = std::max(assessment.riskScore, 0.80f);
            assessment.actions.push_back(WatchdogAction::RealityAnchor);
            reason << "missing_heartbeat:" << subsystem << ";";
        }
    }

    for (const auto& entry : heartbeats) {
        if (now - entry.second > heartbeatTimeout) {
            assessment.healthy = false;
            assessment.riskScore = std::max(assessment.riskScore, 0.75f);
            assessment.actions.push_back(WatchdogAction::RealityAnchor);
            reason << "stale_heartbeat:" << entry.first << ";";
        }
    }

    if (latencyMs > latencyBudgetMs) {
        assessment.healthy = false;
        assessment.riskScore = std::max(assessment.riskScore, static_cast<float>(std::min(1.0, latencyMs / (latencyBudgetMs * 2.0))));
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
    std::lock_guard<std::mutex> lock(mutex_);
    heartbeatTimeoutMs_ = std::max(1.0, timeoutMs);
}

void IndependentWatchdog::setLatencyBudgetMs(double latencyBudgetMs) {
    std::lock_guard<std::mutex> lock(mutex_);
    latencyBudgetMs_ = std::max(1.0, latencyBudgetMs);
}

void IndependentWatchdog::requireSubsystem(const std::string& subsystem) {
    std::lock_guard<std::mutex> lock(mutex_);
    if (!subsystem.empty() &&
        std::find(requiredSubsystems_.begin(), requiredSubsystems_.end(), subsystem) == requiredSubsystems_.end()) {
        requiredSubsystems_.push_back(subsystem);
    }
}

void IndependentWatchdog::requireSubsystems(const std::vector<std::string>& subsystems) {
    for (const auto& subsystem : subsystems) {
        requireSubsystem(subsystem);
    }
}

} // namespace dnxr
