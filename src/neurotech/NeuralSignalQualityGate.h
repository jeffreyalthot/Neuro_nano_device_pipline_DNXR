#pragma once
#include <string>
namespace dnxr { enum class NeuralSignalGateAction { AllowIntentAndStimulus,BlockIntent,BlockStimulus,HoldLastSafeFrame,RealityAnchor,Recalibration }; struct NeuralSignalGateDecision { NeuralSignalGateAction action=NeuralSignalGateAction::BlockStimulus; bool intentAllowed=false,stimulationAllowed=false; std::string reason="not_evaluated"; }; class NeuralSignalQualityGate { public: NeuralSignalGateDecision evaluate(float signalQuality, bool dropout, float artifactProbability, int repeatedArtifacts) const; }; }
