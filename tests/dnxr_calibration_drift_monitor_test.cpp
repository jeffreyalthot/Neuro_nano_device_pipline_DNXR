#include "../src/calibration/CalibrationDriftMonitor.h"

#include <iostream>
#include <string>
#include <vector>

namespace {

bool require(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "dnxr_calibration_drift_monitor_test: " << message << '\n';
        return false;
    }
    return true;
}

} // namespace

int main() {
    dnxr::CalibrationDriftMonitor monitor;
    const std::vector<float> baselineSignal{0.10F, 0.12F, 0.09F, 0.11F};
    const auto baseline = monitor.buildBaseline(baselineSignal, 0.95F, 100.0, "unit_baseline");
    monitor.setBaseline(baseline);

    const auto stable = monitor.assess({0.11F, 0.12F, 0.10F, 0.12F}, 0.94F, 0.90F, 0.10F, 101.0);
    if (!require(stable.reason == "calibration_stable", "stable signal should stay calibrated") ||
        !require(!stable.requiresRecalibration, "stable signal should not require recalibration") ||
        !require(stable.driftScore < 0.45F, "stable drift score should remain low")) {
        return 1;
    }

    const auto velocity = monitor.assess({0.95F, 0.90F, 0.85F, 0.80F}, 0.50F, 0.40F, 0.80F, 101.5);
    if (!require(velocity.requiresRecalibration, "fast drift should require recalibration") ||
        !require(velocity.blockCriticalActions, "fast drift should block critical actions") ||
        !require(velocity.reason == "calibration_drift_velocity_high", "fast drift should identify velocity")) {
        return 1;
    }

    dnxr::CalibrationDriftMonitor agedMonitor;
    agedMonitor.setBaseline(baseline);
    const auto aged = agedMonitor.assess({0.10F, 0.12F, 0.09F, 0.11F}, 0.95F, 0.95F, 0.10F, 2100.0);
    if (!require(aged.requiresRecalibration, "old calibration should require recalibration") ||
        !require(!aged.blockCriticalActions, "old but stable calibration should not block critical actions") ||
        !require(aged.reason == "calibration_age_recalibration_due", "old calibration should report age")) {
        return 1;
    }

    dnxr::CalibrationDriftMonitor emptyMonitor;
    const auto missing = emptyMonitor.assess({}, 0.0F, 0.0F, 1.0F, 1.0);
    if (!require(missing.requiresRecalibration, "missing baseline should require recalibration") ||
        !require(missing.blockCriticalActions, "missing baseline should block critical actions") ||
        !require(missing.confidenceEntropy == 1.0F, "missing baseline should mark entropy high")) {
        return 1;
    }

    return 0;
}
