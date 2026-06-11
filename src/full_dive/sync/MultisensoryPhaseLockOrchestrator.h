#pragma once
#include "neural_chip/NeuralChipFrame.h"
#include "full_dive/sync/NeuralChipLatencyBudget.h"
namespace dnxr {
class MultisensoryPhaseLockOrchestrator {
public:
    FullDiveChipFrame synchronize(const FullDiveChipFrame& frame) const;
    FullDiveChipFrame synchronize(const FullDiveChipFrame& frame, const NeuralChipLatencyBudget& budget) const;
    PhaseLockDecision decide(const FullDiveChipFrame& frame, const NeuralChipLatencyBudget& budget = {}) const;
};
}
