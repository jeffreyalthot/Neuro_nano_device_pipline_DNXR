#pragma once

#include "full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.h"

#include <string>
#include <vector>

namespace dnxr {

struct PipelineConfigLoadResult {
    bool accepted = false;
    NeuralChipOnlyPipelineConfig config{};
    std::vector<std::string> errors{};
    std::vector<std::string> warnings{};
};

class NeuralChipPipelineConfigLoader {
public:
    static PipelineConfigLoadResult loadResultFromFile(const std::string& path);
    static PipelineConfigLoadResult loadResultFromJsonText(const std::string& text);

    static NeuralChipOnlyPipelineConfig loadFromFile(const std::string& path);
    static NeuralChipOnlyPipelineConfig loadFromJsonText(const std::string& text);
};

} // namespace dnxr
