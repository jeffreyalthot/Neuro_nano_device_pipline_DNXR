#include "RuntimeInferenceStage.h"

namespace dnxr {

const char* RuntimeInferenceStage::name() {
    return "inference";
}

RuntimeStageStatus RuntimeInferenceStage::validate(bool healthy, const std::string& failureReason) {
    return healthy ? RuntimeStageStatus::ok(name()) : RuntimeStageStatus::degraded(failureReason);
}

RuntimePipelineFrame RuntimeInferenceStage::execute(RuntimePipelineFrame frame, const Input& input) {
    if (!frame.continueCycle) {
        return frame;
    }
    if (input.heartbeatInference) {
        input.heartbeatInference();
    }
    if (input.inferSymbolicState) {
        frame.symbolicState = input.inferSymbolicState(frame.features, frame.timestamp);
    }
    frame.biometric = input.buildBiometricFrame ? input.buildBiometricFrame(frame.telemetry) : BiometricFrame{};
    if (input.runInferenceTick) {
        frame.cognitive = input.runInferenceTick(frame.cleanSignals, frame.biometric);
    }
    if (input.runAdaptationTick) {
        frame.adaptation = input.runAdaptationTick(frame.cognitive, frame.biometric);
    }
    if (input.lastDiagnostics) {
        frame.diagnostics = input.lastDiagnostics();
    }
    frame.inferenceStatus = validate(true, "inference_stage_unhealthy");
    return frame;
}

} // namespace dnxr
