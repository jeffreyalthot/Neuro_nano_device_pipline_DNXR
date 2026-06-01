#pragma once

#include "FullDiveRuntimeSnapshot.h"
#include "../safety/SafetyStateMachine.h"

namespace dnxr {

class FullDiveSafetyBridge {
public:
    FullDiveRuntimeSnapshot evaluate(const FullDiveSynchronizationEngine& engine) const;
    void apply(const FullDiveRuntimeSnapshot& snapshot, SafetyStateMachine& stateMachine) const;
};

} // namespace dnxr
