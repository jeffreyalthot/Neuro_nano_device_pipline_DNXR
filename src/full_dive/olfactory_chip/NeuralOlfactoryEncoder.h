#pragma once
#include "neural_chip/NeuralChipFrame.h"
namespace dnxr {
struct VirtualOdorField { float presence01=0.0f, identityCluster01=0.0f, pleasantness01=0.0f, freshness01=0.0f, smoke01=0.0f, chemical01=0.0f, decay01=0.0f, habituation01=0.0f, respirationPhase01=0.0f; };
struct PatientOlfactoryMap { bool experimentalChannelEnabled=false; float lowIntensityLimit01=0.20f; };
struct OlfactorySafetyState { bool isolateChannel=true; float nauseaRisk01=0.0f; float trigeminalRisk01=0.0f; };
class NeuralOlfactoryEncoder { public: NeuralOlfactoryVector encode(const VirtualOdorField& odor,const PatientOlfactoryMap& map,const OlfactorySafetyState& safety) const; };
}
