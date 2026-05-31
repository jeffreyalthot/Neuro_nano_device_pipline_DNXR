#pragma once

#include <cstddef>
#include <string>
#include <vector>

#include "../network/NeuralIntentProtocol.h"
#include "../safety/NeuroConsentManager.h"
#include "../cognition/InducedVectorInfluenceEngine.h"
#include "../safety/AddictionRiskGuard.h"

namespace dnxr {

struct CognitiveState {
    float attention = 0.0f;
    float fatigue = 0.0f;
    float stress = 0.0f;
    float emotionValence = 0.0f;
    float motorIntent = 0.0f;
    float immersionIndex = 0.0f;
};

struct BiometricFrame {
    float heartRate = 0.0f;
    float respirationRate = 0.0f;
    float galvanicSkinResponse = 0.0f;
    float eyeFocus = 0.0f;
    float postureStability = 0.0f;
    float movementEnergy = 0.0f;
};

struct XRAdaptationFrame {
    float sceneIntensity = 0.0f;
    float musicArousal = 0.0f;
    float hapticsDensity = 0.0f;
    float narrativePressure = 0.0f;
    float distortionAmount = 0.0f;
    float visualClarity = 0.0f;
    float depthPrecision = 0.0f;
    float motionStability = 0.0f;
    float contrastEnhancement = 0.0f;
    std::string shaderProfile;
};


struct XRActionCommand {
    float moveX = 0.0f;
    float moveY = 0.0f;
    float lookYaw = 0.0f;
    float lookPitch = 0.0f;
    float interactStrength = 0.0f;
    bool jump = false;
    bool grab = false;
};



struct XRFullBodyCommand {
    float headYaw = 0.0f;
    float headPitch = 0.0f;
    float torsoYaw = 0.0f;
    float torsoLean = 0.0f;
    float leftArmLift = 0.0f;
    float rightArmLift = 0.0f;
    float leftHandGrip = 0.0f;
    float rightHandGrip = 0.0f;
    float leftLegStride = 0.0f;
    float rightLegStride = 0.0f;
    float locomotionForward = 0.0f;
    float locomotionLateral = 0.0f;
    float balanceAssist = 0.0f;
};

struct NeuroPipelineDiagnostics {
    std::string streamSource;
    std::string brainwaveClass;
    std::string neurofeedbackCue;
    std::string safetyMode;
    float biometricFusionScore = 0.0f;
    float stressAdaptationGain = 0.0f;
    float attentionLock = 0.0f;
    float neuralSignalQuality = 0.0f;
    float neuralSignalVariance = 0.0f;
    float visualHarmonyIndex = 0.0f;
    float intentConfidence = 0.0f;
    float intentAmbiguityMargin = 0.0f;
    bool intentAmbiguityDetected = false;
    bool intentFallbackRecommended = false;
    bool intentExecutionAuthorized = false;
    int intentConfirmationCount = 0;
    float jitterCompensation = 0.0f;
    std::string fallbackChannel;
    std::string intentSchemaVersion;
    std::string intentActionToken{"none"};
    std::string authorizedIntentToken{"none"};
    float inducedInfluence = 0.0f;
    float decisionDrift = 0.0f;
    float autonomyPreservation = 1.0f;
    float addictionRisk = 0.0f;
    bool addictionMitigationActive = false;
    std::string addictionPolicyAction;
};

class NeuralIntentDecoder { public: float decode(const std::vector<float>& neuralWindow) const; };
class EmotionInferenceEngine { public: float inferValence(const std::vector<float>& neuralWindow, const BiometricFrame& biometrics) const; };
class CognitiveLoadPredictor { public: float predictLoad(const std::vector<float>& neuralBandPower, const BiometricFrame& biometrics) const; };
class AdaptiveRealityModel { public: XRAdaptationFrame adapt(const CognitiveState& state, const BiometricFrame& biometrics) const; };
class NeuralChipSignalAcquisition {
public:
    std::vector<float> ingestFrame(const std::vector<float>& rawFrame, const std::string& source) const;
    float estimateSignalQuality(const std::vector<float>& frame) const;
    float estimateVariance(const std::vector<float>& frame) const;
};
class NoiseReductionPipeline { public: std::vector<float> denoise(const std::vector<float>& frame) const; };
class FrequencyBandAnalyzer { public: std::vector<float> extractBands(const std::vector<float>& frame) const; };
class BrainwaveClassifier { public: std::string classify(const std::vector<float>& bandPower) const; };
class NeurofeedbackController { public: std::string buildFeedbackCue(const CognitiveState& state) const; };
class BiometricFusionEngine { public: float fusionScore(const CognitiveState& state, const BiometricFrame& biometrics) const; };
class StressAdaptationModel { public: float adaptationGain(float stress, float heartRate) const; };
class AttentionTrackingSystem { public: float attentionLock(float attention, float eyeFocus, float postureStability) const; };

enum class RuntimePlatformProfile { Generic, Windows, Linux, MacOS };

class DeepImmersionPlatform {
public:
    DeepImmersionPlatform();

    CognitiveState runInferenceTick(const std::vector<float>& rawNeuralVectorFrame, const BiometricFrame& biometrics, const std::string& source = "neural_chip_stream");
    XRAdaptationFrame runAdaptationTick(const CognitiveState& state, const BiometricFrame& biometrics) const;
    XRActionCommand runActionTick(const std::vector<float>& neuralVectorFrame, const CognitiveState& state) const;
    XRFullBodyCommand runFullBodyControlTick(const std::vector<float>& neuralVectorFrame, const CognitiveState& state);
    void setRuntimePlatformProfile(RuntimePlatformProfile profile);
    RuntimePlatformProfile runtimePlatformProfile() const;
    NeuroPipelineDiagnostics lastDiagnostics() const;
    void setConsentPolicy(const NeuroConsentPolicy& policy);
    void setInducedVector(const std::vector<float>& inducedVector);

private:
    std::vector<float> buildTemporalWindow(const std::vector<float>& filteredFrame);
    void appendHistory(const std::vector<float>& filteredFrame);

    NeuralChipSignalAcquisition acquisition_;
    NoiseReductionPipeline denoise_;
    FrequencyBandAnalyzer bandAnalyzer_;
    NeuralIntentDecoder intent_;
    EmotionInferenceEngine emotion_;
    CognitiveLoadPredictor cognitiveLoad_;
    AdaptiveRealityModel adaptiveReality_;
    BrainwaveClassifier classifier_;
    NeurofeedbackController neurofeedback_;
    BiometricFusionEngine fusion_;
    StressAdaptationModel stressAdaptation_;
    AttentionTrackingSystem attentionTracking_;
    NeuralIntentProtocol intentProtocol_;
    NeuroConsentManager consentManager_;
    InducedVectorInfluenceEngine influenceEngine_;
    std::vector<float> inducedVector_;
    AddictionRiskGuard addictionGuard_;

    XRFullBodyCommand smoothFullBodyCommand(const XRFullBodyCommand& current);

    std::vector<float> neuralHistory_;
    std::size_t historyLimit_;
    NeuroPipelineDiagnostics lastDiagnostics_;
    XRFullBodyCommand lastBodyCommand_;
    std::string lastIntentToken_{"none"};
    int intentConfirmationCount_ = 0;
    RuntimePlatformProfile platformProfile_ = RuntimePlatformProfile::Generic;
};

} // namespace dnxr
