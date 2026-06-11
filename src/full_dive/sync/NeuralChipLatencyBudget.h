#pragma once

#include "neural_chip/NeuralChipFrame.h"

namespace dnxr {

struct NeuralChipLatencyBudget {
    float visualLatencyMs = 0.0f;
    float auditoryLatencyMs = 0.0f;
    float touchLatencyMs = 0.0f;
    float proprioceptionLatencyMs = 0.0f;
    float vestibularLatencyMs = 0.0f;
    float motorLatencyMs = 0.0f;
    float olfactoryLatencyMs = 0.0f;
    float gustatoryLatencyMs = 0.0f;
    float audioVisualSkewMs = 0.0f;
    float visualTouchSkewMs = 0.0f;
    float motorProprioceptionSkewMs = 0.0f;
    float visualVestibularSkewMs = 0.0f;
};

struct PhaseLockDecision {
    bool allowFrame = true;
    bool holdLastSafeFrame = false;
    bool reduceMotion = false;
    bool muteStartle = false;
    bool neutralizeVestibular = false;
    float syncRisk01 = 0.0f;
};

class NeuralChipLatencyBudgetEvaluator {
public:
    PhaseLockDecision evaluate(const NeuralChipLatencyBudget& budget) const;
};

} // namespace dnxr
