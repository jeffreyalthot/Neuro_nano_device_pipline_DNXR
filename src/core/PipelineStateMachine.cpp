#include "PipelineStateMachine.h"
namespace dnxr {
bool PipelineStateMachine::transition(PipelineState next, std::string reason) {
    const bool ok = (state_ == PipelineState::Booting && next == PipelineState::Ready) ||
        (state_ == PipelineState::Ready && (next == PipelineState::Running || next == PipelineState::Shutdown)) ||
        (state_ == PipelineState::Running && (next == PipelineState::WaitingForAck || next == PipelineState::Hold || next == PipelineState::EmergencyLatched || next == PipelineState::Shutdown)) ||
        (state_ == PipelineState::WaitingForAck && (next == PipelineState::Running || next == PipelineState::Hold || next == PipelineState::EmergencyLatched)) ||
        (state_ == PipelineState::Hold && (next == PipelineState::Running || next == PipelineState::EmergencyLatched || next == PipelineState::Shutdown)) ||
        (state_ == PipelineState::EmergencyLatched && next == PipelineState::Shutdown);
    if (ok) { state_ = next; lastReason_ = std::move(reason); }
    else { lastReason_ = "invalid transition from " + toString(state_) + " to " + toString(next) + ": " + reason; }
    return ok;
}
std::string PipelineStateMachine::toString(PipelineState state) { switch(state){ case PipelineState::Booting: return "booting"; case PipelineState::Ready: return "ready"; case PipelineState::Running: return "running"; case PipelineState::WaitingForAck: return "waiting_for_ack"; case PipelineState::Hold: return "hold"; case PipelineState::EmergencyLatched: return "emergency_latched"; case PipelineState::Shutdown: return "shutdown";} return "unknown"; }
}
