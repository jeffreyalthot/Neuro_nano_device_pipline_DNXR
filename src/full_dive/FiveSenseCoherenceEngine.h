#pragma once
#include "FullDiveNeuralFrame.h"
#include <string>
namespace dnxr {
enum class CoherenceIntervention { None, ReduceIntensity, RealityAnchor, Decompression, EmergencyStop };
struct FiveSenseCoherenceReport {
    float visualVestibularConflict = 0.0f;
    float audioVisualLatencySkew = 0.0f;
    float hapticProprioceptiveMismatch = 0.0f;
    float smellTasteContextMismatch = 0.0f;
    float bodySchemaMismatch = 0.0f;
    float globalPerceptualCoherence = 1.0f;
    CoherenceIntervention intervention = CoherenceIntervention::None;
    std::string reason = "coherent";
};
class FiveSenseCoherenceEngine { public: FiveSenseCoherenceReport evaluate(const FullDiveNeuralFrame& frame) const; };
} // namespace dnxr
