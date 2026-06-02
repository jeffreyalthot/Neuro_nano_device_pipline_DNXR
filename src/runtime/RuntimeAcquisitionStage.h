#pragma once

#include "RuntimePipelineFrame.h"
#include "RuntimeStageContext.h"
#include "RuntimeStageTypes.h"

#include <functional>
#include <string>
#include <vector>

namespace dnxr {

class RuntimeStageServices;

class RuntimeAcquisitionStage {
public:
    struct Input {
        std::function<std::vector<float>()> pullBioTelemetry;
        std::function<bool(const std::vector<float>&)> enforcePhysiologicalGuard;
        std::function<void()> heartbeatRuntimeCycle;
        std::function<void()> heartbeatAcquisition;
        std::function<std::vector<float>()> readNeuralNodeSignals;
        std::function<NeuralChipFrame()> readChipFrame;
    };

    static const char* name();
    static RuntimeStageStatus validate(bool healthy, const std::string& failureReason);
    static RuntimePipelineFrame execute(const RuntimeStageContext& context, const Input& input);
    static RuntimePipelineFrame execute(const RuntimeStageContext& context, RuntimeStageServices& services);
};

} // namespace dnxr
