#include "../src/neurotech/NeuroSDKBridge.h"

#include <cassert>

int main() {
    void* handle = dnxr_create_platform();
    assert(handle != nullptr);

    const float neuralVector[] = {0.11f, 0.22f, 0.29f, 0.34f, 0.27f, 0.19f};
    DnxrBiometricFrameC bio{};
    bio.heartRate = 74.0f;
    bio.respirationRate = 14.0f;
    bio.galvanicSkinResponse = 0.18f;
    bio.eyeFocus = 0.79f;
    bio.postureStability = 0.82f;
    bio.movementEnergy = 0.22f;

    DnxrCognitiveStateC state{};
    const int okInference = dnxr_run_inference(handle, neuralVector, 6U, bio, "sdk_bridge_test", &state);
    assert(okInference == 1);
    assert(state.immersionIndex >= 0.0f && state.immersionIndex <= 1.0f);

    DnxrXRActionCommandC action{};
    const int okAction = dnxr_run_action(handle, neuralVector, 6U, state, &action);
    assert(okAction == 1);
    assert(action.moveX >= -1.0f && action.moveX <= 1.0f);
    assert(action.interactStrength >= 0.0f && action.interactStrength <= 1.0f);

    DnxrXRFullBodyCommandC body{};
    const int okFullBody = dnxr_run_full_body_action(handle, neuralVector, 6U, state, &body);
    assert(okFullBody == 1);
    assert(body.headYaw >= -1.0f && body.headYaw <= 1.0f);
    assert(body.leftHandGrip >= 0.0f && body.leftHandGrip <= 1.0f);

    DnxrXRAdaptationFrameC frame{};
    const int okAdaptation = dnxr_run_adaptation(handle, state, bio, &frame);
    assert(okAdaptation == 1);
    assert(frame.sceneIntensity >= 0.0f && frame.sceneIntensity <= 1.0f);
    assert(frame.shaderProfile != nullptr);
    assert(frame.visualClarity >= 0.0f && frame.visualClarity <= 1.0f);
    assert(frame.depthPrecision >= 0.0f && frame.depthPrecision <= 1.0f);
    assert(frame.motionStability >= 0.0f && frame.motionStability <= 1.0f);
    assert(frame.contrastEnhancement >= 0.0f && frame.contrastEnhancement <= 1.0f);

    dnxr_destroy_platform(handle);
    return 0;
}
