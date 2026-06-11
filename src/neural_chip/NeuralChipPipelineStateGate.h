#pragma once

#include "neural_chip/NeuralChipAckHandshake.h"
#include "neural_chip/NeuralChipPipelineStateMachine.h"
#include "neural_chip/PipelineResumeController.h"

namespace dnxr {

enum class PipelineHoldKind { None, AwaitingAck, Decompression, Neutral, Minimal, Emergency, Shutdown };

struct PipelineStateDecision {
    bool allowWorldProcessing = true;
    bool allowNormalCommit = true;
    bool requireHold = false;
    PipelineHoldKind holdKind = PipelineHoldKind::None;
    float ramp01 = 1.0f;
    const char* reason = "running";
};

class NeuralChipPipelineStateGate {
public:
    PipelineStateDecision evaluate(const NeuralChipPipelineStateMachine& stateMachine,
                                   const NeuralChipAckHandshake& ackHandshake,
                                   const PipelineResumeController& resumeController,
                                   double nowMs) const;
};

} // namespace dnxr
