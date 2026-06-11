#pragma once

namespace dnxr {

enum class NeuralChipPipelineState {
    ColdStart,
    Running,
    AwaitingAck,
    HoldDecompression,
    HoldNeutral,
    HoldMinimal,
    ControlledResume,
    EmergencyLatched,
    Shutdown
};

class NeuralChipPipelineStateMachine {
public:
    NeuralChipPipelineState state() const;
    const char* reason() const;

    bool start();
    bool onCommitAccepted();
    bool onAckValid();
    bool onAckMissingHold();
    bool onPersistentHold();
    bool onStableHold();
    bool onResumeRampComplete();
    bool triggerEmergency(const char* reason);
    bool onAuditedReset();
    bool shutdown(const char* reason = "shutdown");

    bool allowsNormalFrame() const;
    bool awaitingAck() const;
    bool inHold() const;
    bool emergencyLatched() const;

private:
    bool transition(NeuralChipPipelineState next, const char* reason);
    NeuralChipPipelineState state_ = NeuralChipPipelineState::ColdStart;
    const char* reason_ = "cold start";
};

const char* toString(NeuralChipPipelineState state);

} // namespace dnxr
