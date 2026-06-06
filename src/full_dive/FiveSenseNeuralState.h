#pragma once
#include <cstdint>
namespace dnxr {
struct ModalityNeuralState { float currentIntensity=0,targetIntensity=0,latencyMs=0,jitterMs=0,confidence=0,calibrationConfidence=0,signalQuality=0,riskScore=1,adaptationLevel=0,fatigueLevel=0; std::uint64_t lastAppliedTimestamp=0,lastFeedbackTimestamp=0; };
struct FiveSenseNeuralState { ModalityNeuralState visualState,auditoryState,somatosensoryState,thermalState,proprioceptiveState,vestibularState,olfactoryState,gustatoryState,interoceptiveState,realityAnchorState; float globalRisk() const; };
}
