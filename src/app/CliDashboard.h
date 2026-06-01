#pragma once

#include "../config/RuntimeConfig.h"

#include <string>
#include <vector>

namespace dnxr {

struct CliDashboardSnapshot {
    int cycle = 0;
    std::string cognitiveState;
    bool safetyOk = true;
    std::vector<float> features;
    std::vector<float> frame;
    std::vector<float> telemetry;
    float calibratedIntensity = 0.0f;
    std::string avatarPose;
    std::string signalSource;
    std::string renderQualityMode = "unknown";
    std::string auditHash = "genesis";
    double lastLatencyMs = 0.0;
    double avgLatencyMs = 0.0;
    double jitterMs = 0.0;
    double maxLatencyMs = 0.0;
    unsigned long long safetyEvents = 0;
};

class CliDashboard {
public:
    explicit CliDashboard(RuntimeConfig config);
    ~CliDashboard();

    void render(const CliDashboardSnapshot& snapshot) const;

private:
    void drawHeader() const;
    void drawSection(const std::string& title) const;

    RuntimeConfig config_;
};

} // namespace dnxr
