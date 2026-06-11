#pragma once
#include "full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.h"
namespace dnxr {
class WorldFrameSanitizer { public: NeuralChipOnlyWorldFrame apply(const NeuralChipOnlyWorldFrame& world, const NeuralChipOnlyPipelineConfig& config) const; };
}
