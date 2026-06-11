#include "full_dive/sync/MultisensoryPhaseLockOrchestrator.h"
#include <algorithm>
namespace dnxr {
PhaseLockDecision MultisensoryPhaseLockOrchestrator::decide(const FullDiveChipFrame& frame, const NeuralChipLatencyBudget& budget) const {
    PhaseLockDecision out = NeuralChipLatencyBudgetEvaluator{}.evaluate(budget);
    const bool conflict = frame.vestibular.visualVestibularMismatch01 > 0.65f ||
                          frame.visual.flickerRisk01 > 0.65f ||
                          frame.auditory.startleRisk01 > 0.65f;
    if (conflict) {
        out.reduceMotion = true;
        out.muteStartle = true;
        out.neutralizeVestibular = true;
        out.syncRisk01 = std::max(out.syncRisk01, 0.70f);
    }
    return out;
}

FullDiveChipFrame MultisensoryPhaseLockOrchestrator::synchronize(const FullDiveChipFrame& frame) const {
    return synchronize(frame, {});
}

FullDiveChipFrame MultisensoryPhaseLockOrchestrator::synchronize(const FullDiveChipFrame& frame, const NeuralChipLatencyBudget& budget) const {
    FullDiveChipFrame out = frame;
    const PhaseLockDecision decision = decide(frame, budget);
    if (decision.neutralizeVestibular) {
        out.vestibular.linearAcceleration01 = 0.0f;
        out.vestibular.angularVelocity01 = 0.0f;
    }
    if (decision.reduceMotion) {
        out.visual.motionFlow01 = std::min(out.visual.motionFlow01, 0.20f);
        out.motorIntent.gaitIntent01 = std::min(out.motorIntent.gaitIntent01, 0.20f);
    }
    if (decision.muteStartle) {
        out.auditory.loudness01 = std::min(out.auditory.loudness01, 0.30f);
        out.auditory.startleRisk01 = std::min(out.auditory.startleRisk01, 0.30f);
    }
    if (!decision.allowFrame) {
        out.safety.aggregateRisk01 = std::max(out.safety.aggregateRisk01, 0.90f);
    } else if (decision.syncRisk01 > 0.0f) {
        out.safety.aggregateRisk01 = std::max(out.safety.aggregateRisk01, decision.syncRisk01);
    }
    return out;
}
}
