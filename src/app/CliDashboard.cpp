#include "CliDashboard.h"
#include "../utils/TerminalControl.h"

#include <iostream>
#include <utility>

namespace dnxr {

CliDashboard::CliDashboard(RuntimeConfig config)
    : config_(std::move(config)) {
    TerminalControl::hideCursor();
}

CliDashboard::~CliDashboard() {
    TerminalControl::showCursor();
}

void CliDashboard::render(const CliDashboardSnapshot& snapshot) const {
    if (!config_.dashboardEnabled) {
        return;
    }

    TerminalControl::clear();
    drawHeader();

    drawSection("SYSTEM");
    std::cout << "Cycle: " << snapshot.cycle << "\n";
    std::cout << "Profile: " << config_.profile << "\n";
    std::cout << "Signal source: " << snapshot.signalSource << "\n";
    std::cout << "State: " << snapshot.cognitiveState << "\n";
    std::cout << "Safety: " << (snapshot.safetyOk ? "OK" : "TRIGGERED") << "\n";
    std::cout << "Audit hash: " << snapshot.auditHash << "\n";

    drawSection("FEATURES");
    if (snapshot.features.size() >= 4U) {
        std::cout << "Mean:  " << snapshot.features[0] << "\n";
        std::cout << "Min:   " << snapshot.features[1] << "\n";
        std::cout << "Max:   " << snapshot.features[2] << "\n";
        std::cout << "Stdev: " << snapshot.features[3] << "\n";
    }

    drawSection("IMMERSION");
    std::cout << "Frame channels: " << snapshot.frame.size() << "\n";
    std::cout << "Intensity: " << snapshot.calibratedIntensity << "\n";
    std::cout << "Avatar pose: " << snapshot.avatarPose << "\n";

    drawSection("BIOMETRICS");
    if (snapshot.telemetry.size() >= 3U) {
        std::cout << "Heart rate: " << snapshot.telemetry[0] << "\n";
        std::cout << "Stress:     " << snapshot.telemetry[1] << "\n";
        std::cout << "Focus:      " << snapshot.telemetry[2] << "\n";
    }

    drawSection("RUNTIME METRICS");
    std::cout << "Last latency ms: " << snapshot.lastLatencyMs << "\n";
    std::cout << "Avg latency ms:  " << snapshot.avgLatencyMs << "\n";
    std::cout << "Jitter ms:       " << snapshot.jitterMs << "\n";
    std::cout << "Max latency ms:  " << snapshot.maxLatencyMs << "\n";
    std::cout << "Safety events:   " << snapshot.safetyEvents << "\n";

    drawSection("RENDER PIPELINE");
    std::cout << "Quality mode: " << snapshot.renderQualityMode << "\n";
    std::cout << "Frame emitted.\n";
}

void CliDashboard::drawHeader() const {
    std::cout
        << "=============================================\n"
        << "   DNXR NEURAL XR RUNTIME DASHBOARD (CLI)\n"
        << "=============================================\n\n";
}

void CliDashboard::drawSection(const std::string& title) const {
    std::cout << "\n[" << title << "]\n";
    std::cout << "---------------------------------------------\n";
}

} // namespace dnxr
