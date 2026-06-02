#pragma once

#include "RuntimePipelineFrame.h"
#include "RuntimeCallback.h"
#include "RuntimeStageTypes.h"

#include <cstdint>
#include <string>
#include <vector>

namespace dnxr {

class RuntimeStageServices;

class RuntimeImmersionFrameStage {
public:
    struct Input {
        RuntimeCallback<XRFullBodyCommand(const std::vector<float>&, const CognitiveState&)> runFullBodyControlTick;
        RuntimeCallback<NeuralSensoryFrame(const CognitiveState&, const XRFullBodyCommand&, const XRAdaptationFrame&, const NeuroPipelineDiagnostics&)> buildImmersiveFrame;
        RuntimeCallback<MultisensoryAlignmentReport(const std::vector<float>&, std::uint64_t, double, double)> alignVector;
        RuntimeCallback<void(std::uint64_t, const std::string&, const std::string&, double, float, bool, const std::string&)> recordTrace;
        RuntimeCallback<void(const std::string&)> logWarn;
        double timestampMs = 0.0;
        double phaseToleranceMs = 2.0;
    };

    static const char* name();
    static RuntimeStageStatus validate(bool healthy, const std::string& failureReason);
    static RuntimePipelineFrame execute(RuntimePipelineFrame frame, const Input& input);
    static RuntimePipelineFrame execute(RuntimePipelineFrame frame, RuntimeStageServices& services);
};

} // namespace dnxr
