#pragma once

#include <functional>
#include <string>

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
    using TransitionObserver = std::function<void(NeuralChipPipelineState, NeuralChipPipelineState, const std::string&)>;

    NeuralChipPipelineState state() const;
    const char* reason() const;
    void setTransitionObserver(TransitionObserver observer);

    bool start();
    bool onCommitAccepted();
    bool onAckValid();
    bool onAckMissingHold();
    bool onPersistentHold();
    bool onStableHold();
    bool onResumeRampComplete();
    bool triggerEmergency(const char* reason);
    bool onAuditedReset(bool operatorAuthorized = true);
    bool shutdown(const char* reason = "shutdown");

    bool allowsNormalFrame() const;
    bool awaitingAck() const;
    bool inHold() const;
    bool emergencyLatched() const;

private:
    bool transition(NeuralChipPipelineState next, const char* reason, bool auditedReset = false, bool operatorAuthorized = false);
    NeuralChipPipelineState state_ = NeuralChipPipelineState::ColdStart;
    std::string reason_ = "cold start";
    TransitionObserver observer_{};
};

const char* toString(NeuralChipPipelineState state);

} // namespace dnxr
