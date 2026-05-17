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

class PhysiologicalGuard {
public:
    PhysiologicalGuard();

    /// Vérifie la stabilité physiologique et déclenche un arrêt si nécessaire.
    bool enforce(const std::vector<float>& telemetry) const;

    bool isEmergencyActive() const;

    uint64_t getViolationCount() const;
    uint64_t getEnforcementCount() const;

    bool getLastDecision() const;

private:
    void triggerEmergency(const std::string& reason) const;

private:
    StabilityMonitor monitor_;
    KillSwitch killSwitch_;

    mutable bool emergencyMode_;
    mutable bool lastDecision_;
    mutable uint64_t lastViolationTime_;
};

} // namespace dnxr