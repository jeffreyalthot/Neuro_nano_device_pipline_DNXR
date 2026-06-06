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

bool isValidTelemetry(const PhysiologicalTelemetryFrame& t) {
    return std::isfinite(t.heartRateBpm) &&
           std::isfinite(t.oxygenSaturationPercent) &&
           std::isfinite(t.respiratoryRateBpm) &&
           std::isfinite(t.systolicBloodPressureMmHg) &&
           std::isfinite(t.diastolicBloodPressureMmHg) &&
           std::isfinite(t.temperatureCelsius) &&
           std::isfinite(t.eegSeizureRisk) &&
           std::isfinite(t.sedationLevel) &&
           t.heartRateBpm >= 35.0f && t.heartRateBpm <= 150.0f &&
           t.oxygenSaturationPercent >= 92.0f &&
           t.respiratoryRateBpm >= 8.0f && t.respiratoryRateBpm <= 30.0f &&
           t.systolicBloodPressureMmHg >= 80.0f && t.systolicBloodPressureMmHg <= 180.0f &&
           t.diastolicBloodPressureMmHg >= 45.0f && t.diastolicBloodPressureMmHg <= 120.0f &&
           t.temperatureCelsius >= 35.0f && t.temperatureCelsius <= 38.5f &&
           t.eegSeizureRisk <= 0.20f &&
           t.sedationLevel <= 0.50f &&
           t.patientCommunicationAvailable &&
           !t.panicSignal &&
           !t.convulsiveSignal &&
           t.consciousnessMonitored;
}

std::vector<float> legacyCardioRespiratoryVector(const PhysiologicalTelemetryFrame& t) {
    return {t.heartRateBpm / 100.0f, t.oxygenSaturationPercent / 100.0f};
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


bool PhysiologicalGuard::enforce(const PhysiologicalTelemetryFrame& telemetry) const {

    std::lock_guard<std::mutex> lock(g_guardMutex);

    ++g_enforcementCount;

    if (!isValidTelemetry(telemetry)) {
        Logger::warn("PhysiologicalGuard: structured monitoring prerequisite violation.");
        ++g_violationCount;
        emergencyState_ = EmergencyState::EmergencyLatched;
        lastDecision_ = false;
        lastEmergencyReason_ = "structured_monitoring_violation";
        triggerEmergency("structured_monitoring_violation");
        return false;
    }

    const bool safe = monitor_.checkCardioRespiratoryWindow(legacyCardioRespiratoryVector(telemetry));
    if (!safe) {
        Logger::warn("PhysiologicalGuard: structured cardio-respiratory threshold violation.");
        ++g_violationCount;
        emergencyState_ = EmergencyState::EmergencyLatched;
        lastDecision_ = false;
        lastEmergencyReason_ = "structured_threshold_violation";
        triggerEmergency("structured_threshold_violation");
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
