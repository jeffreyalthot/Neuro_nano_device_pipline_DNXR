#include "full_dive/sync/NeuralChipLatencyBudget.h"

#include <algorithm>
#include <cmath>

namespace dnxr {
namespace {
float clamp01(float v) { return std::max(0.0f, std::min(1.0f, v)); }
float absf(float v) { return std::fabs(v); }
}

PhaseLockDecision NeuralChipLatencyBudgetEvaluator::evaluate(const NeuralChipLatencyBudget& budget) const {
    const float maxLatency = std::max({budget.visualLatencyMs, budget.auditoryLatencyMs, budget.touchLatencyMs,
                                       budget.proprioceptionLatencyMs, budget.vestibularLatencyMs, budget.motorLatencyMs,
                                       budget.olfactoryLatencyMs, budget.gustatoryLatencyMs});
    const float maxSkew = std::max({absf(budget.audioVisualSkewMs), absf(budget.visualTouchSkewMs),
                                    absf(budget.motorProprioceptionSkewMs), absf(budget.visualVestibularSkewMs)});
    PhaseLockDecision out;
    out.syncRisk01 = clamp01(std::max(maxLatency / 120.0f, maxSkew / 45.0f));
    out.reduceMotion = absf(budget.motorProprioceptionSkewMs) > 25.0f || maxLatency > 80.0f;
    out.muteStartle = absf(budget.audioVisualSkewMs) > 35.0f;
    out.neutralizeVestibular = absf(budget.visualVestibularSkewMs) > 25.0f;
    out.holdLastSafeFrame = out.syncRisk01 > 0.85f;
    out.allowFrame = !out.holdLastSafeFrame;
    return out;
}

} // namespace dnxr
