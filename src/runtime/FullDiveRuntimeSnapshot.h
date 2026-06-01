#pragma once

#include "FullDiveSynchronizationEngine.h"

#include <string>

namespace dnxr {

enum class FullDiveInterventionLevel {
    None,
    ReduceIntensity,
    Decompress,
    RealityAnchor,
    EmergencyStop,
    Lockout
};

struct FullDiveRuntimeSnapshot {
    FullDiveRuntimeState state;
    FullDiveInterventionLevel intervention = FullDiveInterventionLevel::None;
    float severity = 0.0f;
    std::string reason = "full_dive_nominal";
    std::string traceTag = "full_dive:nominal";

    bool safetyInterventionRequired() const {
        return intervention != FullDiveInterventionLevel::None;
    }
};

} // namespace dnxr
