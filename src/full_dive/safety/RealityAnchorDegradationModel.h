#pragma once
#include "neural_chip/NeuralChipFrame.h"
namespace dnxr {
struct RealityAnchorDegradationDecision { bool requireHold=false; bool triggerEmergencyLatch=false; float globalGain01=1.0f; const char* reason="stable reality anchor"; };
class RealityAnchorDegradationModel { public: RealityAnchorDegradationDecision decide(const NeuralRealityAnchorVector& anchor) const; FullDiveChipFrame apply(const FullDiveChipFrame& frame, const RealityAnchorDegradationDecision& decision) const; };
}
