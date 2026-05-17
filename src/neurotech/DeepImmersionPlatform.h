#pragma once

#include <cstddef>
#include <string>
#include <vector>

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
    std::string shaderProfile;
};

struct NeuroPipelineDiagnostics {
    std::string streamSource;
    std::string brainwaveClass;
    std::string neurofeedbackCue;
    float biometricFusionScore = 0.0f;
    float stressAdaptationGain = 0.0f;
    float attentionLock = 0.0f;
};

class NeuralIntentDecoder { public: float decode(const std::vector<float>& eegWindow) const; };
class EmotionInferenceEngine { public: float inferValence(const std::vector<float>& eegWindow, const BiometricFrame& biometrics) const; };
class CognitiveLoadPredictor { public: float predictLoad(const std::vector<float>& eegBandPower, const BiometricFrame& biometrics) const; };
class AdaptiveRealityModel { public: XRAdaptationFrame adapt(const CognitiveState& state, const BiometricFrame& biometrics) const; };
class EEGSignalAcquisition { public: std::vector<float> ingestFrame(const std::vector<float>& rawFrame, const std::string& source) const; };
class NoiseReductionPipeline { public: std::vector<float> denoise(const std::vector<float>& frame) const; };
class FrequencyBandAnalyzer { public: std::vector<float> extractBands(const std::vector<float>& frame) const; };
class BrainwaveClassifier { public: std::string classify(const std::vector<float>& bandPower) const; };
class NeurofeedbackController { public: std::string buildFeedbackCue(const CognitiveState& state) const; };
class BiometricFusionEngine { public: float fusionScore(const CognitiveState& state, const BiometricFrame& biometrics) const; };
class StressAdaptationModel { public: float adaptationGain(float stress, float heartRate) const; };
class AttentionTrackingSystem { public: float attentionLock(float attention, float eyeFocus, float postureStability) const; };

class DeepImmersionPlatform {
public:
    DeepImmersionPlatform();

    CognitiveState runInferenceTick(const std::vector<float>& rawEegFrame, const BiometricFrame& biometrics, const std::string& source = "generic_stream");
    XRAdaptationFrame runAdaptationTick(const CognitiveState& state, const BiometricFrame& biometrics) const;
    NeuroPipelineDiagnostics lastDiagnostics() const;

private:
    std::vector<float> buildTemporalWindow(const std::vector<float>& filteredFrame);
    void appendHistory(const std::vector<float>& filteredFrame);

    EEGSignalAcquisition acquisition_;
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

    std::vector<float> eegHistory_;
    std::size_t historyLimit_;
    NeuroPipelineDiagnostics lastDiagnostics_;
};

} // namespace dnxr
