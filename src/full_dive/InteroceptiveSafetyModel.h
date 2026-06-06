#pragma once
#include <string>
namespace dnxr { enum class InteroceptiveAction { Continue,ReduceIntensity,RealityAnchor,Decompress,EmergencyStop }; struct InteroceptiveSafetyDecision { InteroceptiveAction action=InteroceptiveAction::Continue; std::string reason="stable"; }; class InteroceptiveSafetyModel { public: InteroceptiveSafetyDecision evaluate(float respiration,float heartRate,float stress,bool nausea,bool vertigo,bool panic,bool disorientation,bool stopRequested,bool communicationLost) const; }; }
