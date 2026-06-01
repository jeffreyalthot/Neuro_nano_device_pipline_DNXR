#pragma once

#include "FullDiveSynchronizationEngine.h"

namespace dnxr {

struct FullDiveRuntimeSnapshot {
    FullDiveRuntimeState state;
    bool safetyInterventionRequired = false;
    std::string reason = "full_dive_nominal";
};

} // namespace dnxr
