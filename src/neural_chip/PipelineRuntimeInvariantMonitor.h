#pragma once

#include "neural_chip/NeuralChipAckHandshake.h"
#include "neural_chip/NeuralChipEmergencyLatch.h"
#include "neural_chip/NeuralChipPipelineStateMachine.h"
#include "neural_chip/NeuralChipReplayRecorder.h"

#include <string>

namespace dnxr {

struct PipelineRuntimeInvariantReport {
    bool accepted = true;
    std::string reason;
    bool triggerEmergencyLatch = false;
};

class PipelineRuntimeInvariantMonitor {
public:
    PipelineRuntimeInvariantReport evaluate(const PipelineStepResult& step,
                                            const NeuralChipPipelineStateMachine& stateMachine,
                                            const NeuralChipAckState& ackState,
                                            const NeuralChipEmergencyLatch& latch,
                                            const NeuralChipReplayRecord& replay) const;
};

} // namespace dnxr
