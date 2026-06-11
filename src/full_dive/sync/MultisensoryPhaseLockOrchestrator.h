#pragma once
#include "neural_chip/NeuralChipFrame.h"
namespace dnxr {
class MultisensoryPhaseLockOrchestrator {
public:
    FullDiveChipFrame synchronize(const FullDiveChipFrame& frame) const;
};
}
