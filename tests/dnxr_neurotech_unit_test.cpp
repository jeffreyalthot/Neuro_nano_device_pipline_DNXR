#include "../src/neurotech/DeepImmersionPlatform.h"

#include <cassert>
#include <vector>

int main() {
    dnxr::DeepImmersionPlatform platform;

    dnxr::BiometricFrame biometrics;
    biometrics.heartRate = 78.0f;
    biometrics.respirationRate = 15.0f;
    biometrics.galvanicSkinResponse = 0.2f;
    biometrics.eyeFocus = 0.82f;
    biometrics.postureStability = 0.76f;
    biometrics.movementEnergy = 0.35f;

    std::vector<float> neuralVector = {0.12f, 0.25f, 0.38f, 0.42f, 0.27f, 0.18f, 0.31f};

    dnxr::NeuroConsentPolicy consent{true, true, true, "unit_test"};
    platform.setConsentPolicy(consent);
    platform.setInducedVector({0.08f, -0.03f, 0.04f});

    dnxr::CognitiveState state = platform.runInferenceTick(neuralVector, biometrics, "unit_test_stream");
    dnxr::XRAdaptationFrame xr = platform.runAdaptationTick(state, biometrics);
    dnxr::XRActionCommand action = platform.runActionTick(neuralVector, state);
    dnxr::XRFullBodyCommand body = platform.runFullBodyControlTick(neuralVector, state);
    const dnxr::XRFullBodyCommand body2 = platform.runFullBodyControlTick(neuralVector, state);
    platform.setRuntimePlatformProfile(dnxr::RuntimePlatformProfile::Linux);
    assert(platform.runtimePlatformProfile() == dnxr::RuntimePlatformProfile::Linux);

    assert(state.attention >= 0.0f && state.attention <= 1.0f);
    assert(state.fatigue >= 0.0f && state.fatigue <= 1.0f);
    assert(state.stress >= 0.0f && state.stress <= 1.0f);
    assert(state.emotionValence >= 0.0f && state.emotionValence <= 1.0f);
    assert(state.motorIntent >= 0.0f && state.motorIntent <= 1.0f);
    assert(state.immersionIndex >= 0.0f && state.immersionIndex <= 1.0f);

    assert(xr.sceneIntensity >= 0.0f && xr.sceneIntensity <= 1.0f);
    assert(xr.musicArousal >= 0.0f && xr.musicArousal <= 1.0f);
    assert(xr.hapticsDensity >= 0.0f && xr.hapticsDensity <= 1.0f);
    assert(xr.narrativePressure >= 0.0f && xr.narrativePressure <= 1.0f);
    assert(!xr.shaderProfile.empty());
    assert(xr.visualClarity >= 0.0f && xr.visualClarity <= 1.0f);
    assert(xr.depthPrecision >= 0.0f && xr.depthPrecision <= 1.0f);
    assert(xr.motionStability >= 0.0f && xr.motionStability <= 1.0f);
    assert(xr.contrastEnhancement >= 0.0f && xr.contrastEnhancement <= 1.0f);
    assert(action.moveX >= -1.0f && action.moveX <= 1.0f);
    assert(action.moveY >= -1.0f && action.moveY <= 1.0f);
    assert(action.lookYaw >= -1.0f && action.lookYaw <= 1.0f);
    assert(action.lookPitch >= -1.0f && action.lookPitch <= 1.0f);
    assert(action.interactStrength >= 0.0f && action.interactStrength <= 1.0f);
    assert(body.headYaw >= -1.0f && body.headYaw <= 1.0f);
    assert(body.headPitch >= -1.0f && body.headPitch <= 1.0f);
    assert(body.leftArmLift >= 0.0f && body.leftArmLift <= 1.0f);
    assert(body.rightArmLift >= 0.0f && body.rightArmLift <= 1.0f);
    assert(body.leftHandGrip >= 0.0f && body.leftHandGrip <= 1.0f);
    assert(body.rightHandGrip >= 0.0f && body.rightHandGrip <= 1.0f);
    assert(body.balanceAssist >= 0.0f && body.balanceAssist <= 1.0f);
    assert(body2.balanceAssist >= 0.0f && body2.balanceAssist <= 1.0f);

    dnxr::BrainwaveClassifier classifier;
    const std::string label = classifier.classify({0.2f, 0.35f, 0.4f, 0.55f, 0.65f});
    assert(!label.empty());

    dnxr::NeurofeedbackController neurofeedback;
    const std::string cue = neurofeedback.buildFeedbackCue(state);
    assert(!cue.empty());

    const dnxr::NeuroPipelineDiagnostics diagnostics = platform.lastDiagnostics();
    assert(diagnostics.streamSource == "unit_test_stream");
    assert(!diagnostics.brainwaveClass.empty());
    assert(!diagnostics.neurofeedbackCue.empty());
    assert(diagnostics.biometricFusionScore >= 0.0f && diagnostics.biometricFusionScore <= 1.0f);
    assert(!diagnostics.safetyMode.empty());
    assert(diagnostics.safetyMode == "nominal" || diagnostics.safetyMode == "stability_guard" || diagnostics.safetyMode == "degraded_fallback");
    assert(diagnostics.neuralSignalQuality >= 0.0f && diagnostics.neuralSignalQuality <= 1.0f);
    assert(diagnostics.neuralSignalVariance >= 0.0f && diagnostics.neuralSignalVariance <= 1.0f);
    assert(diagnostics.stressAdaptationGain >= 0.0f && diagnostics.stressAdaptationGain <= 1.0f);
    assert(diagnostics.attentionLock >= 0.0f && diagnostics.attentionLock <= 1.0f);

    assert(diagnostics.intentConfidence >= 0.0f && diagnostics.intentConfidence <= 1.0f);
    assert(!diagnostics.intentSchemaVersion.empty());
    assert(diagnostics.jitterCompensation >= 0.0f && diagnostics.jitterCompensation <= 1.0f);
    assert(!diagnostics.fallbackChannel.empty());
    assert(diagnostics.inducedInfluence >= 0.0f && diagnostics.inducedInfluence <= 1.0f);
    assert(diagnostics.decisionDrift >= 0.0f && diagnostics.decisionDrift <= 1.0f);
    assert(diagnostics.autonomyPreservation >= 0.0f && diagnostics.autonomyPreservation <= 1.0f);
    assert(diagnostics.addictionRisk >= 0.0f && diagnostics.addictionRisk <= 1.0f);
    assert(!diagnostics.addictionPolicyAction.empty());

    return 0;
}


