#pragma once
#include <string>
namespace dnxr { enum class DecompressionState { None,Requested,RampDownVisual,RampDownVestibular,RampDownHaptic,RealityAnchorStrong,NeutralScene,PatientReorientation,PhysicianCheck,Completed,EmergencyCutoff }; class DecompressionStateMachine { public: DecompressionState request(bool emergency); DecompressionState step(); DecompressionState state() const; std::string reason() const; private: DecompressionState state_=DecompressionState::None; std::string reason_="none"; }; }
