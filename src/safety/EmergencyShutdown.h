// EmergencyShutdown.h

#pragma once

#include <cstdint>

namespace dnxr {

class EmergencyShutdown {
public:
    EmergencyShutdown();

    /// Déclenche la procédure d’urgence.
    void trigger() const;

    /// Engage la séquence de stabilisation système.
    void engage() const;

    /// Retourne si le système est stable.
    bool isStable() const noexcept;

    /// Statistiques
    uint64_t getTriggerCount() const;

    /// Mode arrêt critique
    void setHardStopMode(bool enabled);
    bool isHardStopMode() const;

    /// Temps de stabilisation
    void setStabilizationTime(int ms);
    int getStabilizationTime() const;

private:
    mutable int stabilizationTimeMs_;
    mutable uint64_t lastTriggerTime_;
    mutable bool hardStopMode_;
};

} // namespace dnxr