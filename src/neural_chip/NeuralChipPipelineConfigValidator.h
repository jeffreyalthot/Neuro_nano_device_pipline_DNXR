#pragma once

#include "full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.h"

#include <string>
#include <vector>

namespace dnxr {

struct NeuralChipPipelineConfigValidationResult {
    bool accepted = true;
    std::vector<std::string> errors{};
    std::vector<std::string> warnings{};
};

class NeuralChipPipelineConfigValidator {
public:
    static NeuralChipPipelineConfigValidationResult validate(const NeuralChipOnlyPipelineConfig& config,
                                                            const std::vector<std::string>& unknownKeys = {});
};

} // namespace dnxr
