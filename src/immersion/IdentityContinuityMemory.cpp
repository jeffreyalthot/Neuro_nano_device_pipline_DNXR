#include "IdentityContinuityMemory.h"

#include <algorithm>

namespace dnxr {

void IdentityContinuityMemory::save(const std::string& worldState, float comfortGate, const std::string& inferredState) {
    fingerprint_.spatialAnchor = worldState;
    fingerprint_.narrativeAnchor = inferredState;
    fingerprint_.presenceLevel = std::clamp(comfortGate, 0.0F, 1.0F);
}

ImmersionFingerprint IdentityContinuityMemory::restore() const {
    return fingerprint_;
}

} // namespace dnxr
