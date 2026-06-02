#pragma once

#include "RuntimePipelineFrame.h"
#include "RuntimeStageTypes.h"

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace dnxr {

class RuntimeStageServices;

class RuntimeSignalStage {
public:
    struct Input {
        std::function<std::vector<std::uint8_t>(const std::vector<float>&)> encrypt;
        std::function<std::vector<float>(const std::vector<std::uint8_t>&)> decrypt;
        std::function<std::vector<float>(const std::vector<float>&)> uplink;
        std::function<std::vector<float>(const std::vector<float>&)> denoise;
        std::function<std::vector<float>(const std::vector<float>&)> vectorize;
        std::function<void()> heartbeatSignalProcessing;
    };

    static const char* name();
    static RuntimeStageStatus validate(bool healthy, const std::string& failureReason);
    static RuntimePipelineFrame execute(RuntimePipelineFrame frame, const Input& input);
    static RuntimePipelineFrame execute(RuntimePipelineFrame frame, RuntimeStageServices& services);
};

} // namespace dnxr
