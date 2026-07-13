#include "RuntimeInferenceStage.h"

#include "RuntimeStageServices.h"
#include "RuntimeMotorIntentStage.h"
#include "../cognition/StateInferenceEngine.h"
#include "../neurotech/DeepImmersionPlatform.h"
#include "../safety/IndependentWatchdog.h"

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

namespace {

BiometricFrame buildRuntimeBiometricFrame(const std::vector<float>& telemetry) {
    BiometricFrame bio{};
    if (!telemetry.empty()) {
        bio.heartRate = telemetry[0];
        bio.galvanicSkinResponse = telemetry.size() > 1 ? telemetry[1] : 0.0f;
        bio.eyeFocus = telemetry.size() > 2 ? telemetry[2] : 0.0f;
        bio.respirationRate = 12.0f + bio.galvanicSkinResponse * 8.0f;
        bio.postureStability = telemetry.size() > 5 ? telemetry[5] : 0.75f;
        bio.movementEnergy = telemetry.size() > 6 ? telemetry[6] / 128.0f : 0.2f;
    }
    return bio;
}

} // namespace

RuntimePipelineFrame RuntimeInferenceStage::execute(RuntimePipelineFrame frame, RuntimeStageServices& services) {
    if (!frame.continueCycle) {
        return frame;
    }

    services.watchdog.heartbeat("inference");
    frame.symbolicState = services.brain.inferState(frame.features, frame.timestamp);
    frame.biometric = buildRuntimeBiometricFrame(frame.telemetry);
    frame.cognitive = services.deepImmersion.runInferenceTick(frame.cleanSignals, frame.biometric, "runtime_controller");
    frame.adaptation = services.deepImmersion.runAdaptationTick(frame.cognitive, frame.biometric);
    frame.diagnostics = services.deepImmersion.lastDiagnostics();
    frame = RuntimeMotorIntentStage::execute(std::move(frame), &services.motorWorldEgress);

    const bool valid = !frame.symbolicState.empty() &&
        isValidCognitiveState(frame.cognitive) &&
        isValidDiagnostics(frame.diagnostics);
    frame.inferenceStatus = validate(valid, "invalid_inference_output");
    frame.continueCycle = frame.inferenceStatus.continueCycle;
    if (!frame.continueCycle) {
        frame.stopReason = frame.inferenceStatus.reason;
    }
    return frame;
}

} // namespace dnxr
