#include "DeepImmersionPlatform.h"

#include <algorithm>
#include <numeric>

namespace dnxr {
namespace {
float clamp01(float value) { return std::clamp(value, 0.0f, 1.0f); }
float meanOf(const std::vector<float>& v) {
    if (v.empty()) return 0.0f;
    return std::accumulate(v.begin(), v.end(), 0.0f) / static_cast<float>(v.size());
}
}

float NeuralIntentDecoder::decode(const std::vector<float>& eegWindow) const {
    if (eegWindow.empty()) return 0.0f;
    float absEnergy = 0.0f;
    for (float s : eegWindow) absEnergy += std::abs(s);
    return clamp01(absEnergy / static_cast<float>(eegWindow.size()) * 0.15f);
}

float EmotionInferenceEngine::inferValence(const std::vector<float>& eegWindow, const BiometricFrame& biometrics) const {
    const float corticalTone = clamp01(0.5f + meanOf(eegWindow) * 0.15f);
    const float autonomicPenalty = clamp01(0.6f * biometrics.galvanicSkinResponse + 0.4f * (biometrics.heartRate / 180.0f));
    return clamp01(corticalTone - autonomicPenalty * 0.35f + biometrics.eyeFocus * 0.05f);
}

float CognitiveLoadPredictor::predictLoad(const std::vector<float>& eegBandPower, const BiometricFrame& biometrics) const {
    const float bandMean = meanOf(eegBandPower);
    const float metabolic = clamp01((biometrics.respirationRate / 32.0f) + (biometrics.heartRate / 240.0f));
    return clamp01((bandMean * 0.62f) + (metabolic * 0.38f));
}

XRAdaptationFrame AdaptiveRealityModel::adapt(const CognitiveState& state, const BiometricFrame& biometrics) const {
    XRAdaptationFrame frame;
    frame.sceneIntensity = clamp01(0.35f + state.immersionIndex * 0.65f - state.fatigue * 0.25f);
    frame.musicArousal = clamp01(0.25f + state.attention * 0.30f + state.emotionValence * 0.45f);
    frame.hapticsDensity = clamp01(state.motorIntent * 0.65f + biometrics.movementEnergy * 0.35f);
    frame.narrativePressure = clamp01(state.stress * 0.65f + state.attention * 0.35f);
    frame.distortionAmount = clamp01(state.stress * 0.7f + state.fatigue * 0.3f);
    frame.shaderProfile = (state.stress > 0.70f) ? "stabilizing_calm" : ((state.immersionIndex > 0.72f) ? "deep_focus_hyperreal" : "balanced_dynamic");
    return frame;
}

std::vector<float> EEGSignalAcquisition::ingestFrame(const std::vector<float>& rawFrame, const std::string& source) const {
    if (source.empty() || rawFrame.empty()) return {};
    std::vector<float> clamped;
    clamped.reserve(rawFrame.size());
    for (float sample : rawFrame) clamped.push_back(std::clamp(sample, -200.0f, 200.0f));
    return clamped;
}

std::vector<float> NoiseReductionPipeline::denoise(const std::vector<float>& frame) const {
    if (frame.size() < 5) return frame;
    std::vector<float> out(frame.size(), 0.0f);
    out[0] = frame[0]; out[1] = frame[1];
    out[frame.size() - 2] = frame[frame.size() - 2]; out.back() = frame.back();
    for (std::size_t i = 2; i + 2 < frame.size(); ++i) {
        out[i] = (frame[i - 2] + 2.0f * frame[i - 1] + 3.0f * frame[i] + 2.0f * frame[i + 1] + frame[i + 2]) / 9.0f;
    }
    return out;
}

std::vector<float> FrequencyBandAnalyzer::extractBands(const std::vector<float>& frame) const {
    if (frame.empty()) return {};
    float low=0, mid=0, high=0;
    for (std::size_t i=0;i<frame.size();++i) {
        const float a = std::abs(frame[i]);
        if (i % 3 == 0) low += a;
        else if (i % 3 == 1) mid += a;
        else high += a;
    }
    const float norm = std::max(1.0f, static_cast<float>(frame.size()) * 20.0f);
    return {clamp01(low / norm), clamp01((low + mid * 0.5f) / norm), clamp01(mid / norm), clamp01((mid + high * 0.5f) / norm), clamp01(high / norm)};
}

std::string BrainwaveClassifier::classify(const std::vector<float>& bandPower) const {
    if (bandPower.size() < 5) return "unknown";
    const float alphaBeta = bandPower[2] + bandPower[3];
    const float thetaDelta = bandPower[0] + bandPower[1];
    if (alphaBeta > thetaDelta * 1.4f) return "focused";
    if (thetaDelta > alphaBeta * 1.5f) return "fatigued";
    if (bandPower[4] > 0.45f) return "high_arousal";
    return "neutral";
}

std::string NeurofeedbackController::buildFeedbackCue(const CognitiveState& state) const {
    if (state.stress > 0.75f) return "breathe_and_reduce_fx";
    if (state.fatigue > 0.72f) return "micro_rest_window";
    if (state.attention < 0.38f) return "anchor_attention_target";
    if (state.immersionIndex > 0.80f) return "maintain_deep_dive";
    return "adaptive_balance";
}

float BiometricFusionEngine::fusionScore(const CognitiveState& state, const BiometricFrame& b) const {
    const float phys = clamp01((b.heartRate / 190.0f + b.eyeFocus + b.postureStability + (1.0f - b.galvanicSkinResponse)) / 4.0f);
    return clamp01(phys * 0.5f + state.immersionIndex * 0.5f);
}

float StressAdaptationModel::adaptationGain(float stress, float heartRate) const { return clamp01(stress * 0.7f + (heartRate / 210.0f) * 0.3f); }
float AttentionTrackingSystem::attentionLock(float attention, float eyeFocus, float postureStability) const { return clamp01(attention * 0.45f + eyeFocus * 0.35f + postureStability * 0.2f); }

DeepImmersionPlatform::DeepImmersionPlatform() : historyLimit_(256) {}

void DeepImmersionPlatform::appendHistory(const std::vector<float>& filteredFrame) {
    eegHistory_.insert(eegHistory_.end(), filteredFrame.begin(), filteredFrame.end());
    if (eegHistory_.size() > historyLimit_) {
        const std::size_t trim = eegHistory_.size() - historyLimit_;
        eegHistory_.erase(eegHistory_.begin(), eegHistory_.begin() + static_cast<std::ptrdiff_t>(trim));
    }
}

std::vector<float> DeepImmersionPlatform::buildTemporalWindow(const std::vector<float>& filteredFrame) {
    appendHistory(filteredFrame);
    std::vector<float> window = eegHistory_;
    if (window.size() < 16) {
        window.insert(window.end(), filteredFrame.begin(), filteredFrame.end());
    }
    return window;
}

CognitiveState DeepImmersionPlatform::runInferenceTick(const std::vector<float>& rawEegFrame, const BiometricFrame& biometrics, const std::string& source) {
    const std::vector<float> acquired = acquisition_.ingestFrame(rawEegFrame, source);
    const std::vector<float> filtered = denoise_.denoise(acquired);
    const std::vector<float> temporal = buildTemporalWindow(filtered);
    const std::vector<float> bands = bandAnalyzer_.extractBands(temporal);

    CognitiveState state;
    state.motorIntent = intent_.decode(temporal);
    state.emotionValence = emotion_.inferValence(temporal, biometrics);
    state.fatigue = cognitiveLoad_.predictLoad(bands, biometrics);
    state.attention = clamp01(1.0f - state.fatigue * 0.65f + biometrics.eyeFocus * 0.25f);
    state.stress = clamp01((1.0f - state.emotionValence) * 0.45f + biometrics.galvanicSkinResponse * 0.55f);
    state.immersionIndex = clamp01((state.attention * 0.30f) + (state.motorIntent * 0.25f) + (state.emotionValence * 0.30f) + ((1.0f - state.stress) * 0.15f));

    lastDiagnostics_.streamSource = source;
    lastDiagnostics_.brainwaveClass = classifier_.classify(bands);
    lastDiagnostics_.neurofeedbackCue = neurofeedback_.buildFeedbackCue(state);
    lastDiagnostics_.biometricFusionScore = fusion_.fusionScore(state, biometrics);
    lastDiagnostics_.stressAdaptationGain = stressAdaptation_.adaptationGain(state.stress, biometrics.heartRate);
    lastDiagnostics_.attentionLock = attentionTracking_.attentionLock(state.attention, biometrics.eyeFocus, biometrics.postureStability);

    return state;
}

XRAdaptationFrame DeepImmersionPlatform::runAdaptationTick(const CognitiveState& state, const BiometricFrame& biometrics) const { return adaptiveReality_.adapt(state, biometrics); }
NeuroPipelineDiagnostics DeepImmersionPlatform::lastDiagnostics() const { return lastDiagnostics_; }

} // namespace dnxr
