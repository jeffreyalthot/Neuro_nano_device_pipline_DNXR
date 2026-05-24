#include "NeuroSDKBridge.h"

#include <vector>
#include <string>

namespace {

dnxr::BiometricFrame toCpp(const DnxrBiometricFrameC& b) {
    dnxr::BiometricFrame out;
    out.heartRate = b.heartRate;
    out.respirationRate = b.respirationRate;
    out.galvanicSkinResponse = b.galvanicSkinResponse;
    out.eyeFocus = b.eyeFocus;
    out.postureStability = b.postureStability;
    out.movementEnergy = b.movementEnergy;
    return out;
}

DnxrCognitiveStateC toC(const dnxr::CognitiveState& s) {
    return DnxrCognitiveStateC{s.attention, s.fatigue, s.stress, s.emotionValence, s.motorIntent, s.immersionIndex};
}

dnxr::CognitiveState fromC(const DnxrCognitiveStateC& s) {
    dnxr::CognitiveState out;
    out.attention = s.attention;
    out.fatigue = s.fatigue;
    out.stress = s.stress;
    out.emotionValence = s.emotionValence;
    out.motorIntent = s.motorIntent;
    out.immersionIndex = s.immersionIndex;
    return out;
}

}

extern "C" {

void* dnxr_create_platform() {
    return new dnxr::DeepImmersionPlatform();
}

void dnxr_destroy_platform(void* handle) {
    delete static_cast<dnxr::DeepImmersionPlatform*>(handle);
}

int dnxr_run_inference(void* handle, const float* neuralVector, unsigned int neuralVectorCount, DnxrBiometricFrameC biometrics, const char* source, DnxrCognitiveStateC* outState) {
    if (!handle || !neuralVector || neuralVectorCount == 0 || !outState) return 0;

    auto* platform = static_cast<dnxr::DeepImmersionPlatform*>(handle);
    std::vector<float> neuralVec(neuralVector, neuralVector + neuralVectorCount);
    const std::string src = (source && source[0] != '\0') ? source : "c_api_neural_chip_stream";
    const dnxr::CognitiveState state = platform->runInferenceTick(neuralVec, toCpp(biometrics), src);
    *outState = toC(state);
    return 1;
}


int dnxr_run_action(void* handle, const float* neuralVector, unsigned int neuralVectorCount, DnxrCognitiveStateC state, DnxrXRActionCommandC* outAction) {
    if (!handle || !neuralVector || neuralVectorCount == 0 || !outAction) return 0;

    auto* platform = static_cast<dnxr::DeepImmersionPlatform*>(handle);
    std::vector<float> neuralVec(neuralVector, neuralVector + neuralVectorCount);
    const dnxr::XRActionCommand action = platform->runActionTick(neuralVec, fromC(state));

    outAction->moveX = action.moveX;
    outAction->moveY = action.moveY;
    outAction->lookYaw = action.lookYaw;
    outAction->lookPitch = action.lookPitch;
    outAction->interactStrength = action.interactStrength;
    outAction->jump = action.jump ? 1 : 0;
    outAction->grab = action.grab ? 1 : 0;
    return 1;
}


int dnxr_run_full_body_action(void* handle, const float* neuralVector, unsigned int neuralVectorCount, DnxrCognitiveStateC state, DnxrXRFullBodyCommandC* outBody) {
    if (!handle || !neuralVector || neuralVectorCount == 0 || !outBody) return 0;

    auto* platform = static_cast<dnxr::DeepImmersionPlatform*>(handle);
    std::vector<float> neuralVec(neuralVector, neuralVector + neuralVectorCount);
    const dnxr::XRFullBodyCommand body = platform->runFullBodyControlTick(neuralVec, fromC(state));

    outBody->headYaw = body.headYaw;
    outBody->headPitch = body.headPitch;
    outBody->torsoYaw = body.torsoYaw;
    outBody->torsoLean = body.torsoLean;
    outBody->leftArmLift = body.leftArmLift;
    outBody->rightArmLift = body.rightArmLift;
    outBody->leftHandGrip = body.leftHandGrip;
    outBody->rightHandGrip = body.rightHandGrip;
    outBody->leftLegStride = body.leftLegStride;
    outBody->rightLegStride = body.rightLegStride;
    outBody->locomotionForward = body.locomotionForward;
    outBody->locomotionLateral = body.locomotionLateral;
    outBody->balanceAssist = body.balanceAssist;
    return 1;
}

int dnxr_run_adaptation(void* handle, DnxrCognitiveStateC state, DnxrBiometricFrameC biometrics, DnxrXRAdaptationFrameC* outFrame) {
    if (!handle || !outFrame) return 0;

    auto* platform = static_cast<dnxr::DeepImmersionPlatform*>(handle);
    const dnxr::XRAdaptationFrame frame = platform->runAdaptationTick(fromC(state), toCpp(biometrics));

    static thread_local std::string shaderCache;
    shaderCache = frame.shaderProfile;

    outFrame->sceneIntensity = frame.sceneIntensity;
    outFrame->musicArousal = frame.musicArousal;
    outFrame->hapticsDensity = frame.hapticsDensity;
    outFrame->narrativePressure = frame.narrativePressure;
    outFrame->distortionAmount = frame.distortionAmount;
    outFrame->visualClarity = frame.visualClarity;
    outFrame->depthPrecision = frame.depthPrecision;
    outFrame->motionStability = frame.motionStability;
    outFrame->contrastEnhancement = frame.contrastEnhancement;
    outFrame->shaderProfile = shaderCache.c_str();
    return 1;
}

}
