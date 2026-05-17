#include "../src/neurotech/NeuroSDKBridge.h"

#include <cassert>

int main() {
    void* handle = dnxr_create_platform();
    assert(handle != nullptr);

    const float eeg[] = {0.11f, 0.22f, 0.29f, 0.34f, 0.27f, 0.19f};
    DnxrBiometricFrameC bio{};
    bio.heartRate = 74.0f;
    bio.respirationRate = 14.0f;
    bio.galvanicSkinResponse = 0.18f;
    bio.eyeFocus = 0.79f;
    bio.postureStability = 0.82f;
    bio.movementEnergy = 0.22f;

    DnxrCognitiveStateC state{};
    const int okInference = dnxr_run_inference(handle, eeg, 6U, bio, "sdk_bridge_test", &state);
    assert(okInference == 1);
    assert(state.immersionIndex >= 0.0f && state.immersionIndex <= 1.0f);

    DnxrXRAdaptationFrameC frame{};
    const int okAdaptation = dnxr_run_adaptation(handle, state, bio, &frame);
    assert(okAdaptation == 1);
    assert(frame.sceneIntensity >= 0.0f && frame.sceneIntensity <= 1.0f);
    assert(frame.shaderProfile != nullptr);

    dnxr_destroy_platform(handle);
    return 0;
}
