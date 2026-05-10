#pragma once

#include <string>

namespace dnxr {

struct ImmersionFingerprint {
    std::string spatialAnchor;
    std::string narrativeAnchor;
    float presenceLevel;
};

class IdentityContinuityMemory {
public:
    void save(const std::string& worldState, float comfortGate, const std::string& inferredState);
    ImmersionFingerprint restore() const;

private:
    ImmersionFingerprint fingerprint_{"spawn_point", "intro_arc", 0.65F};
};

} // namespace dnxr
