#pragma once

#include "FullDiveSynchronizationEngine.h"

#include <cstdint>
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

struct FullDiveInterventionMemory {
    std::uint32_t reduceIntensityCount = 0;
    std::uint32_t decompressionCount = 0;
    std::uint32_t realityAnchorCount = 0;
    std::uint32_t emergencyStopCount = 0;
    std::uint32_t lockoutCount = 0;
    double windowSeconds = 60.0;
    double cooldownSeconds = 1.0;
};

struct FullDiveRuntimeSnapshot {
    FullDiveRuntimeState state;
    FullDiveInterventionLevel intervention = FullDiveInterventionLevel::None;
    float severity = 0.0f;
    std::string reason = "full_dive_nominal";
    std::string traceTag = "full_dive:nominal";
    FullDiveInterventionMemory memory;

    bool safetyInterventionRequired() const {
        return intervention != FullDiveInterventionLevel::None;
    }
};

} // namespace dnxr
