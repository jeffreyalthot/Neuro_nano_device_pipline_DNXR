#pragma once

#include "DeepImmersionPlatform.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct DnxrBiometricFrameC {
    float heartRate;
    float respirationRate;
    float galvanicSkinResponse;
    float eyeFocus;
    float postureStability;
    float movementEnergy;
} DnxrBiometricFrameC;

typedef struct DnxrCognitiveStateC {
    float attention;
    float fatigue;
    float stress;
    float emotionValence;
    float motorIntent;
    float immersionIndex;
} DnxrCognitiveStateC;

typedef struct DnxrXRAdaptationFrameC {
    float sceneIntensity;
    float musicArousal;
    float hapticsDensity;
    float narrativePressure;
    float distortionAmount;
    const char* shaderProfile;
} DnxrXRAdaptationFrameC;

void* dnxr_create_platform();
void dnxr_destroy_platform(void* handle);

int dnxr_run_inference(
    void* handle,
    const float* eeg,
    unsigned int eegCount,
    DnxrBiometricFrameC biometrics,
    const char* source,
    DnxrCognitiveStateC* outState);

int dnxr_run_adaptation(
    void* handle,
    DnxrCognitiveStateC state,
    DnxrBiometricFrameC biometrics,
    DnxrXRAdaptationFrameC* outFrame);

#ifdef __cplusplus
}
#endif
