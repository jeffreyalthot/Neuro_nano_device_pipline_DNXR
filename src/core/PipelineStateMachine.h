#pragma once
#include <string>
namespace dnxr { enum class PipelineState { Booting, Ready, Running, WaitingForAck, Hold, EmergencyLatched, Shutdown }; class PipelineStateMachine { public: bool transition(PipelineState next, std::string reason); PipelineState state() const noexcept { return state_; } const std::string& lastReason() const noexcept { return lastReason_; } static std::string toString(PipelineState state); private: PipelineState state_ = PipelineState::Booting; std::string lastReason_ = "initial"; }; }
