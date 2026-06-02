#include "RuntimeInferenceStage.h"

#include <cmath>

namespace dnxr {
namespace {

bool isFinite01(float value) {
    return std::isfinite(value) && value >= 0.0f && value <= 1.0f;
}

} // namespace

const char* RuntimeInferenceStage::name() {
    return "inference";
}

RuntimeStageStatus RuntimeInferenceStage::validate(bool healthy, const std::string& failureReason) {
    return healthy ? RuntimeStageStatus::ok(name()) : RuntimeStageStatus::degraded(failureReason);
}

bool RuntimeInferenceStage::isValidCognitiveState(const CognitiveState& cognitive) {
    return isFinite01(cognitive.immersionIndex) &&
           isFinite01(cognitive.stress) &&
           isFinite01(cognitive.fatigue) &&
           std::isfinite(cognitive.attention) &&
           std::isfinite(cognitive.emotionValence) &&
           std::isfinite(cognitive.motorIntent);
}

bool RuntimeInferenceStage::isValidDiagnostics(const NeuroPipelineDiagnostics& diagnostics) {
    return isFinite01(diagnostics.neuralSignalQuality) &&
           isFinite01(diagnostics.intentConfidence) &&
           isFinite01(diagnostics.autonomyPreservation) &&
           isFinite01(diagnostics.decisionDrift) &&
           std::isfinite(diagnostics.biometricFusionScore) &&
           std::isfinite(diagnostics.stressAdaptationGain) &&
           std::isfinite(diagnostics.attentionLock);
}

RuntimePipelineFrame RuntimeInferenceStage::execute(RuntimePipelineFrame frame, const Input& input) {
    if (!frame.continueCycle) {
        return frame;
    }
    frame.inferenceStatus = validate(
        input.inferSymbolicState && input.buildBiometricFrame && input.runInferenceTick &&
            input.runAdaptationTick && input.lastDiagnostics,
        "inference_stage_unavailable");
    if (!frame.inferenceStatus.continueCycle) {
        frame.continueCycle = false;
        frame.stopReason = frame.inferenceStatus.reason;
        return frame;
    }

    if (input.heartbeatInference) {
        input.heartbeatInference();
    }
    frame.symbolicState = input.inferSymbolicState(frame.features, frame.timestamp);
    frame.biometric = input.buildBiometricFrame(frame.telemetry);
    frame.cognitive = input.runInferenceTick(frame.cleanSignals, frame.biometric);
    frame.adaptation = input.runAdaptationTick(frame.cognitive, frame.biometric);
    frame.diagnostics = input.lastDiagnostics();

    if (frame.symbolicState.empty() ||
        !isValidCognitiveState(frame.cognitive) ||
        !isValidDiagnostics(frame.diagnostics)) {
        frame.inferenceStatus = RuntimeStageStatus::degraded("invalid_inference_output");
        frame.continueCycle = false;
        frame.stopReason = "invalid_inference_output";
        return frame;
    }

    frame.inferenceStatus = validate(true, "inference_stage_unhealthy");
    return frame;
}

} // namespace dnxr
