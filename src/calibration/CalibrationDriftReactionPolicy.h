#pragma once
#include <string>
namespace dnxr { enum class CalibrationDriftAction { ReduceAdaptiveGain,SuppressModality,RealityAnchorRecalibration,EmergencyStop }; struct CalibrationDriftReaction { CalibrationDriftAction action=CalibrationDriftAction::ReduceAdaptiveGain; std::string reason="low_drift"; }; class CalibrationDriftReactionPolicy { public: CalibrationDriftReaction react(float driftScore) const; }; }
