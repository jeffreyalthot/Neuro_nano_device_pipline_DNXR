#pragma once

#include <chrono>
#include <map>
#include <string>
#include <vector>

namespace dnxr {

enum class WatchdogAction {
    None,
    ReduceIntensiveRendering,
    RealityAnchor,
    ZeroStimulation,
    LockSession
};

struct WatchdogAssessment {
    bool healthy = true;
    bool requiresEmergencyStop = false;
    float riskScore = 0.0f;
    std::vector<WatchdogAction> actions;
    std::string reason = "watchdog_nominal";
};

class IndependentWatchdog {
public:
    explicit IndependentWatchdog(double heartbeatTimeoutMs = 250.0, double latencyBudgetMs = 25.0);

    void heartbeat(const std::string& subsystem, std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now());
    WatchdogAssessment assess(
        double latencyMs,
        bool neuralSignalValid,
        float biometricRisk,
        float driftScore,
        std::chrono::steady_clock::time_point now = std::chrono::steady_clock::now()) const;

    void setHeartbeatTimeoutMs(double timeoutMs);
    void setLatencyBudgetMs(double latencyBudgetMs);

private:
    double heartbeatTimeoutMs_;
    double latencyBudgetMs_;
    std::map<std::string, std::chrono::steady_clock::time_point> heartbeats_;
};

} // namespace dnxr
