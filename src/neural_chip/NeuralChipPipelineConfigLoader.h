#pragma once

#include "full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.h"

#include <string>

namespace dnxr {

class NeuralChipPipelineConfigLoader {
public:
    static NeuralChipOnlyPipelineConfig loadFromFile(const std::string& path);
    static NeuralChipOnlyPipelineConfig loadFromJsonText(const std::string& text);
};

} // namespace dnxr
