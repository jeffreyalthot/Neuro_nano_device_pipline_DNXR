#pragma once

#include "RuntimePipelineFrame.h"
#include "RuntimeCallback.h"
#include "RuntimeStageTypes.h"

#include <cstdint>
#include <string>
#include <vector>

namespace dnxr {

class RuntimeStageServices;

class RuntimeSignalStage {
public:
    struct Input {
        RuntimeCallback<std::vector<std::uint8_t>(const std::vector<float>&)> encrypt;
        RuntimeCallback<std::vector<float>(const std::vector<std::uint8_t>&)> decrypt;
        RuntimeCallback<std::vector<float>(const std::vector<float>&)> uplink;
        RuntimeCallback<std::vector<float>(const std::vector<float>&)> denoise;
        RuntimeCallback<std::vector<float>(const std::vector<float>&)> vectorize;
        RuntimeCallback<void()> heartbeatSignalProcessing;
    };

    static const char* name();
    static RuntimeStageStatus validate(bool healthy, const std::string& failureReason);
    static RuntimePipelineFrame execute(RuntimePipelineFrame frame, const Input& input);
    static RuntimePipelineFrame execute(RuntimePipelineFrame frame, RuntimeStageServices& services);
};

} // namespace dnxr
