#pragma once

#include "neural_chip/NeuralChipFrame.h"
#include "neural_chip/PatientSpecificNeuralMap.h"

namespace dnxr {

class PatientModalityPolicyGate {
public:
    FullDiveChipFrame apply(const FullDiveChipFrame& frame, const PatientSpecificNeuralMap& map) const;

private:
    static float clamp01(float value);
    static float modalityScale(const PatientModalityProfile& profile);
    static void scale(NeuralVisualVector& value, float gain);
    static void scale(NeuralAuditoryVector& value, float gain);
    static void scale(NeuralTouchVector& value, float gain);
    static void scale(NeuralProprioceptionVector& value, float gain);
    static void scale(NeuralVestibularVector& value, float gain);
    static void scale(NeuralMotorIntentVector& value, float gain);
    static void scale(NeuralOlfactoryVector& value, float gain);
    static void scale(NeuralGustatoryVector& value, float gain);
};

} // namespace dnxr
