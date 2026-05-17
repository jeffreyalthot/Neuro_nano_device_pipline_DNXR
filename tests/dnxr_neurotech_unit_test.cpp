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

    std::vector<float> eeg = {0.12f, 0.25f, 0.38f, 0.42f, 0.27f, 0.18f, 0.31f};

    dnxr::CognitiveState state = platform.runInferenceTick(eeg, biometrics, "unit_test_stream");
    dnxr::XRAdaptationFrame xr = platform.runAdaptationTick(state, biometrics);

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
    assert(diagnostics.stressAdaptationGain >= 0.0f && diagnostics.stressAdaptationGain <= 1.0f);
    assert(diagnostics.attentionLock >= 0.0f && diagnostics.attentionLock <= 1.0f);

    return 0;
}
