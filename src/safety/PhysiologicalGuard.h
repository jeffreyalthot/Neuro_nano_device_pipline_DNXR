// PhysiologicalGuard.h

#pragma once

#include <vector>
#include <string>
#include <cstdint>

#include "StabilityMonitor.h"

namespace dnxr {

class KillSwitch {
public:
    void trigger() const {
        // Safe immediate shutdown hook (placeholder for real system halt)
    }
};

struct PhysiologicalTelemetryFrame {
    PhysiologicalTelemetryFrame() {}

    float heartRateBpm = 0.0f;
    float oxygenSaturationPercent = 0.0f;
    float respiratoryRateBpm = 0.0f;
    float systolicBloodPressureMmHg = 0.0f;
    float diastolicBloodPressureMmHg = 0.0f;
    float temperatureCelsius = 0.0f;
    float eegSeizureRisk = 1.0f;
    float sedationLevel = 1.0f;
    bool patientCommunicationAvailable = false;
    bool panicSignal = true;
    bool convulsiveSignal = true;
    bool consciousnessMonitored = false;
};

class PhysiologicalGuard {
public:
    enum class EmergencyState {
        Nominal = 0,
        Warning = 1,
        EmergencyLatched = 2
    };

    PhysiologicalGuard();

    /// Vérifie la stabilité physiologique et déclenche un arrêt si nécessaire.
    bool enforce(const std::vector<float>& telemetry) const;
    bool enforce(const PhysiologicalTelemetryFrame& telemetry) const;

    bool isEmergencyActive() const;

    uint64_t getViolationCount() const;
    uint64_t getEnforcementCount() const;

    bool getLastDecision() const;
    EmergencyState getEmergencyState() const;
    std::string getLastEmergencyReason() const;

private:
    void triggerEmergency(const std::string& reason) const;

private:
    StabilityMonitor monitor_;
    KillSwitch killSwitch_;

    mutable bool emergencyMode_;
    mutable bool lastDecision_;
    mutable uint64_t lastViolationTime_;
    mutable EmergencyState emergencyState_;
    mutable std::string lastEmergencyReason_;
};

} // namespace dnxr
