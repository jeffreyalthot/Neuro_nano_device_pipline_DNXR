#pragma once

#include "neural_chip/NeuralChipFrame.h"

namespace dnxr {

struct PerModalityPerceptionError {
    float visualError01 = 0.0f;
    float auditoryError01 = 0.0f;
    float touchError01 = 0.0f;
    float proprioceptionError01 = 0.0f;
    float vestibularError01 = 0.0f;
    float olfactoryError01 = 0.0f;
    float gustatoryError01 = 0.0f;
    float motorError01 = 0.0f;
};

struct ClosedLoopCorrection {
    float visualGain01 = 1.0f;
    float auditoryGain01 = 1.0f;
    float touchGain01 = 1.0f;
    float vestibularGain01 = 1.0f;
    float chemicalGain01 = 0.0f;
    bool requireHold = false;
};

class ClosedLoopPerceptionErrorController {
public:
    PerModalityPerceptionError analyze(const FullDiveChipFrame& sentFrame, const ChipReadFrame& feedback) const;
    ClosedLoopCorrection decide(const PerModalityPerceptionError& error);
    FullDiveChipFrame applyCorrection(const FullDiveChipFrame& frame, const ClosedLoopCorrection& correction) const;
    void reset();

private:
    static float clamp01(float value);
    PerModalityPerceptionError previous_{};
};

} // namespace dnxr
