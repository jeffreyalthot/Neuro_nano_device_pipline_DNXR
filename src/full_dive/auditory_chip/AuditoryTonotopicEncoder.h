#pragma once
#include "neural_chip/NeuralChipFrame.h"
namespace dnxr {
struct SpatialAudioFrame { float loudness01=0.0f, lowBand01=0.0f, midBand01=0.0f, highBand01=0.0f, attack01=0.0f, sustain01=0.0f, leftRight01=0.5f, frontBack01=0.5f, distance01=0.0f, speech01=0.0f, danger01=0.0f; };
struct PatientAuditoryMap { float tonotopicGain01=1.0f; };
struct AuditorySafetyState { bool allowAuditoryStimulus=true; float startleRisk01=0.0f; };
class AuditoryTonotopicEncoder { public: NeuralAuditoryVector encode(const SpatialAudioFrame& audio,const PatientAuditoryMap& map,const AuditorySafetyState& safety) const; };
}
