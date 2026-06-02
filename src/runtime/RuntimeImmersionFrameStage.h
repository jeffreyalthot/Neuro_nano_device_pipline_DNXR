#pragma once

#include "RuntimePipelineFrame.h"
#include "RuntimeStageTypes.h"

#include <cstdint>
#include <functional>
#include <string>
#include <vector>

namespace dnxr {

class RuntimeStageServices;

class RuntimeImmersionFrameStage {
public:
    struct Input {
        std::function<XRFullBodyCommand(const std::vector<float>&, const CognitiveState&)> runFullBodyControlTick;
        std::function<NeuralSensoryFrame(const CognitiveState&, const XRFullBodyCommand&, const XRAdaptationFrame&, const NeuroPipelineDiagnostics&)> buildImmersiveFrame;
        std::function<MultisensoryAlignmentReport(const std::vector<float>&, std::uint64_t, double, double)> alignVector;
        std::function<void(std::uint64_t, const std::string&, const std::string&, double, float, bool, const std::string&)> recordTrace;
        std::function<void(const std::string&)> logWarn;
        double timestampMs = 0.0;
        double phaseToleranceMs = 2.0;
    };

    static const char* name();
    static RuntimeStageStatus validate(bool healthy, const std::string& failureReason);
    static RuntimePipelineFrame execute(RuntimePipelineFrame frame, const Input& input);
    static RuntimePipelineFrame execute(RuntimePipelineFrame frame, RuntimeStageServices& services);
};

} // namespace dnxr
