#pragma once

#include "neural_chip/NeuralChipFrame.h"

namespace dnxr {

struct MotorIntentConfidenceDecision {
    bool lowConfidence = false;
    bool freezeAvatar = false;
    bool safetyPoseRequired = false;
    const char* reason = "";
};

class MotorIntentConfidenceGate {
public:
    NeuralMotorIntentVector apply(const NeuralMotorIntentVector& intent, float signalQuality01, MotorIntentConfidenceDecision* decision = nullptr) const;

private:
    static float clamp01(float value);
};

} // namespace dnxr
