#pragma once

#include "RuntimePipelineFrame.h"
#include "RuntimeCallback.h"
#include "RuntimeStageTypes.h"

#include <string>
#include <vector>

namespace dnxr {

class RuntimeStageServices;

class RuntimeInferenceStage {
public:
    struct Input {
        RuntimeCallback<std::string(const std::vector<float>&, double)> inferSymbolicState;
        RuntimeCallback<BiometricFrame(const std::vector<float>&)> buildBiometricFrame;
        RuntimeCallback<CognitiveState(const std::vector<float>&, const BiometricFrame&)> runInferenceTick;
        RuntimeCallback<XRAdaptationFrame(const CognitiveState&, const BiometricFrame&)> runAdaptationTick;
        RuntimeCallback<NeuroPipelineDiagnostics()> lastDiagnostics;
        RuntimeCallback<void()> heartbeatInference;
    };

    static const char* name();
    static RuntimeStageStatus validate(bool healthy, const std::string& failureReason);
    static bool isValidCognitiveState(const CognitiveState& cognitive);
    static bool isValidDiagnostics(const NeuroPipelineDiagnostics& diagnostics);
    static RuntimePipelineFrame execute(RuntimePipelineFrame frame, const Input& input);
    static RuntimePipelineFrame execute(RuntimePipelineFrame frame, RuntimeStageServices& services);
};

} // namespace dnxr
