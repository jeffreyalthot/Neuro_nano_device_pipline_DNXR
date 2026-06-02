#pragma once

#include "RuntimePipelineFrame.h"
#include "RuntimeStageTypes.h"

#include <functional>
#include <string>
#include <vector>

namespace dnxr {

class RuntimeStageServices;

class RuntimeStimulusStage {
public:
    struct Input {
        std::function<SafeNeuralStimulusCommand(std::uint64_t, const std::vector<float>&, const SafetyDecision&, double)> buildCommand;
        std::function<void()> heartbeatStimulus;
        std::function<bool(const SafeNeuralStimulusCommand&)> writeSafeStimulus;
        std::function<void(const std::string&)> emergencyStopHardware;
        std::function<std::vector<float>(const std::vector<float>&)> downlink;
        std::function<void(const std::vector<float>&)> writeNeuralStimulus;
    };

    static const char* name();
    static RuntimeStageStatus validate(bool healthy, const std::string& failureReason);
    static RuntimePipelineFrame execute(RuntimePipelineFrame frame, const Input& input);
    static RuntimePipelineFrame execute(RuntimePipelineFrame frame, RuntimeStageServices& services);
};

} // namespace dnxr
