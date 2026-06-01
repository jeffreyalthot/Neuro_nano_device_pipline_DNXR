#pragma once

#include "FullDiveRuntimeSnapshot.h"
#include "../safety/SafetyStateMachine.h"

#include <chrono>
#include <deque>
#include <mutex>

namespace dnxr {

class FullDiveSafetyBridge {
public:
    explicit FullDiveSafetyBridge(FullDiveInterventionMemory memory = FullDiveInterventionMemory{});

    FullDiveRuntimeSnapshot evaluateInstant(const FullDiveSynchronizationEngine& engine) const;
    FullDiveRuntimeSnapshot observeAndEscalate(const FullDiveRuntimeSnapshot& instantSnapshot);
    FullDiveRuntimeSnapshot evaluateAndRemember(const FullDiveSynchronizationEngine& engine);
    FullDiveRuntimeSnapshot evaluate(const FullDiveSynchronizationEngine& engine);
    void apply(const FullDiveRuntimeSnapshot& snapshot, SafetyStateMachine& stateMachine) const;
    SafetyDecision mergeIntoDecision(
        const FullDiveRuntimeSnapshot& snapshot,
        const SafetyDecision& baseDecision) const;

    FullDiveInterventionMemory memory() const;
    void resetMemory();

private:
    void rememberLocked(FullDiveInterventionLevel level, std::chrono::steady_clock::time_point now);
    std::uint32_t countWindowLocked(FullDiveInterventionLevel level, std::chrono::steady_clock::time_point now) const;

private:
    struct InterventionIncident {
        FullDiveInterventionLevel level;
        std::chrono::steady_clock::time_point timestamp;
    };

    mutable std::mutex mutex_;
    FullDiveInterventionMemory memory_;
    std::deque<InterventionIncident> incidents_;
    std::chrono::steady_clock::time_point lastInterventionAt_{};
};

} // namespace dnxr
