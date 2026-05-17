// TimeSync.h

#pragma once

#include <cstdint>

namespace dnxr {

class TimeSync {
public:
    /// Timestamp monotone en millisecondes (séquencement stable)
    static std::int64_t nowMs();

    /// Réinitialise le générateur temporel interne
    static void reset();

    /// Retourne le dernier timestamp généré
    static std::int64_t lastTimestamp();

private:
    static std::int64_t generateRaw();
};

} // namespace dnxr