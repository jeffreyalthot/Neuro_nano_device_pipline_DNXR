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
    float visualClarity;
    float depthPrecision;
    float motionStability;
    float contrastEnhancement;
    const char* shaderProfile;
} DnxrXRAdaptationFrameC;



typedef struct DnxrXRFullBodyCommandC {
    float headYaw;
    float headPitch;
    float torsoYaw;
    float torsoLean;
    float leftArmLift;
    float rightArmLift;
    float leftHandGrip;
    float rightHandGrip;
    float leftLegStride;
    float rightLegStride;
    float locomotionForward;
    float locomotionLateral;
    float balanceAssist;
} DnxrXRFullBodyCommandC;

typedef struct DnxrXRActionCommandC {
    float moveX;
    float moveY;
    float lookYaw;
    float lookPitch;
    float interactStrength;
    int jump;
    int grab;
} DnxrXRActionCommandC;

void* dnxr_create_platform();
void dnxr_destroy_platform(void* handle);

int dnxr_run_inference(
    void* handle,
    const float* neuralVector,
    unsigned int neuralVectorCount,
    DnxrBiometricFrameC biometrics,
    const char* source,
    DnxrCognitiveStateC* outState);

int dnxr_run_action(
    void* handle,
    const float* neuralVector,
    unsigned int neuralVectorCount,
    DnxrCognitiveStateC state,
    DnxrXRActionCommandC* outAction);

int dnxr_run_full_body_action(
    void* handle,
    const float* neuralVector,
    unsigned int neuralVectorCount,
    DnxrCognitiveStateC state,
    DnxrXRFullBodyCommandC* outBody);

int dnxr_run_adaptation(
    void* handle,
    DnxrCognitiveStateC state,
    DnxrBiometricFrameC biometrics,
    DnxrXRAdaptationFrameC* outFrame);

#ifdef __cplusplus
}
#endif
