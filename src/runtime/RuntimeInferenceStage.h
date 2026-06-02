#pragma once

#include "RuntimePipelineFrame.h"
#include "RuntimeStageTypes.h"

#include <functional>
#include <string>
#include <vector>

namespace dnxr {

class RuntimeInferenceStage {
public:
    struct Input {
        std::function<std::string(const std::vector<float>&, double)> inferSymbolicState;
        std::function<BiometricFrame(const std::vector<float>&)> buildBiometricFrame;
        std::function<CognitiveState(const std::vector<float>&, const BiometricFrame&)> runInferenceTick;
        std::function<XRAdaptationFrame(const CognitiveState&, const BiometricFrame&)> runAdaptationTick;
        std::function<NeuroPipelineDiagnostics()> lastDiagnostics;
        std::function<void()> heartbeatInference;
    };

    static const char* name();
    static RuntimeStageStatus validate(bool healthy, const std::string& failureReason);
    static bool isValidCognitiveState(const CognitiveState& cognitive);
    static bool isValidDiagnostics(const NeuroPipelineDiagnostics& diagnostics);
    static RuntimePipelineFrame execute(RuntimePipelineFrame frame, const Input& input);
};

} // namespace dnxr
