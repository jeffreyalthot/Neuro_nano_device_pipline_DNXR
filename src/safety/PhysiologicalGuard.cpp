// PhysiologicalGuard.cpp

#include "PhysiologicalGuard.h"

#include <algorithm>
#include <mutex>
#include <atomic>
#include <chrono>
#include <cmath>

#include "../utils/Logger.h"

namespace dnxr {

namespace {

std::mutex g_guardMutex;

std::atomic<uint64_t> g_violationCount{0};
std::atomic<uint64_t> g_enforcementCount{0};

bool isValidTelemetry(const std::vector<float>& t) {
    if (t.size() < 2) {
        return false;
    }

    return std::isfinite(t[0]) && std::isfinite(t[1]);
}

} // namespace

PhysiologicalGuard::PhysiologicalGuard()
    : emergencyMode_(false),
      lastDecision_(true),
      lastViolationTime_(0),
      emergencyState_(EmergencyState::Nominal),
      lastEmergencyReason_("none") {
}

bool PhysiologicalGuard::enforce(const std::vector<float>& telemetry) const {

    std::lock_guard<std::mutex> lock(g_guardMutex);

    ++g_enforcementCount;

    if (!isValidTelemetry(telemetry)) {
        Logger::warn("PhysiologicalGuard: invalid telemetry detected.");
        emergencyState_ = EmergencyState::EmergencyLatched;
        lastEmergencyReason_ = "invalid_telemetry";
        triggerEmergency("invalid_telemetry");
        return false;
    }

    bool safe = monitor_.checkCardioRespiratoryWindow(telemetry);

    if (!safe) {
        Logger::warn("PhysiologicalGuard: safety threshold violation.");

        ++g_violationCount;
        lastDecision_ = false;
        lastViolationTime_ =
            static_cast<uint64_t>(
                std::chrono::high_resolution_clock::now()
                    .time_since_epoch()
                    .count());

        killSwitch_.trigger();
        emergencyMode_ = true;
        emergencyState_ = EmergencyState::EmergencyLatched;
        lastEmergencyReason_ = "threshold_violation";

        return false;
    }

    lastDecision_ = true;
    emergencyMode_ = false;
    emergencyState_ = EmergencyState::Nominal;
    lastEmergencyReason_ = "none";

    return true;
}

void PhysiologicalGuard::triggerEmergency(const std::string& reason) const {
    Logger::error("EMERGENCY TRIGGERED: " + reason);
    killSwitch_.trigger();
    emergencyMode_ = true;
}

bool PhysiologicalGuard::isEmergencyActive() const {
    return emergencyMode_;
}

PhysiologicalGuard::EmergencyState PhysiologicalGuard::getEmergencyState() const {
    return emergencyState_;
}

std::string PhysiologicalGuard::getLastEmergencyReason() const {
    return lastEmergencyReason_;
}

uint64_t PhysiologicalGuard::getViolationCount() const {
    return g_violationCount.load();
}

uint64_t PhysiologicalGuard::getEnforcementCount() const {
    return g_enforcementCount.load();
}

bool PhysiologicalGuard::getLastDecision() const {
    return lastDecision_;
}

} // namespace dnxr
