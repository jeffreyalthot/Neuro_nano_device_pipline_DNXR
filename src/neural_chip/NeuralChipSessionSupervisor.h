#pragma once

#include "neural_chip/NeuralChipFrame.h"
#include "neural_chip/NeuralChipSessionState.h"

namespace dnxr {

struct SessionSupervisorDecision {
    bool reduceAllModalities = false;
    bool requireHold = false;
    bool triggerEmergencyLatch = false;
    float globalGain01 = 1.0f;
    const char* reason = "";
};

class NeuralChipSessionSupervisor {
public:
    SessionSupervisorDecision decide(const NeuralChipSessionState& state, const FullDiveChipFrame& frame);
    FullDiveChipFrame apply(const FullDiveChipFrame& frame, const SessionSupervisorDecision& decision) const;
    void reset();

private:
    static float clamp01(float value);
    std::uint32_t lowClosedLoopFrames_ = 0;
};

} // namespace dnxr
