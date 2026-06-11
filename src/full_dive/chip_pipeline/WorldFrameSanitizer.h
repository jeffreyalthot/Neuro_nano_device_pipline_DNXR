#pragma once
#include "full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.h"
namespace dnxr {
struct WorldFrameSanitizationResult {
    bool accepted = false;
    bool fatalLifecycleInput = false;
    const char* reason = "not sanitized";
    NeuralChipOnlyWorldFrame sanitized{};
};

class WorldFrameSanitizer {
public:
    WorldFrameSanitizationResult sanitize(const NeuralChipOnlyWorldFrame& world, const NeuralChipOnlyPipelineConfig& config) const;
    NeuralChipOnlyWorldFrame apply(const NeuralChipOnlyWorldFrame& world, const NeuralChipOnlyPipelineConfig& config) const;
};
}
