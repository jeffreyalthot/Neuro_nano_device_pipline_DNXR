#pragma once

#include "RuntimePipelineFrame.h"
#include "RuntimeCallback.h"
#include "RuntimeStageTypes.h"

#include <string>
#include <vector>

namespace dnxr {

class RuntimeStageServices;

class RuntimeStimulusStage {
public:
    struct Input {
        RuntimeCallback<SafeNeuralStimulusCommand(std::uint64_t, const std::vector<float>&, const SafetyDecision&, double)> buildCommand;
        RuntimeCallback<void()> heartbeatStimulus;
        RuntimeCallback<bool(const SafeNeuralStimulusCommand&)> writeSafeStimulus;
        RuntimeCallback<void(const std::string&)> emergencyStopHardware;
        RuntimeCallback<std::vector<float>(const std::vector<float>&)> downlink;
        RuntimeCallback<void(const std::vector<float>&)> writeNeuralStimulus;
    };

    static const char* name();
    static RuntimeStageStatus validate(bool healthy, const std::string& failureReason);
    static RuntimePipelineFrame execute(RuntimePipelineFrame frame, const Input& input);
    static RuntimePipelineFrame execute(RuntimePipelineFrame frame, RuntimeStageServices& services);
};

} // namespace dnxr
