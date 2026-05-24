#include "DeepImmersionPlatform.h"

#include <algorithm>
#include <numeric>
#include <cmath>

namespace dnxr {
namespace {
float clamp01(float value) { return std::clamp(value, 0.0f, 1.0f); }
float meanOf(const std::vector<float>& v) {
    if (v.empty()) return 0.0f;
    return std::accumulate(v.begin(), v.end(), 0.0f) / static_cast<float>(v.size());
}

float varianceOf(const std::vector<float>& v) {
    if (v.size() < 2) return 0.0f;
    const float m = meanOf(v);
    float acc = 0.0f;
    for (float x : v) {
        const float d = x - m;
        acc += d * d;
    }
    return acc / static_cast<float>(v.size() - 1);
}
}

float NeuralIntentDecoder::decode(const std::vector<float>& neuralWindow) const {
    if (neuralWindow.empty()) return 0.0f;
    float absEnergy = 0.0f;
    for (float s : neuralWindow) absEnergy += std::abs(s);
    return clamp01(absEnergy / static_cast<float>(neuralWindow.size()) * 0.15f);
}

float EmotionInferenceEngine::inferValence(const std::vector<float>& neuralWindow, const BiometricFrame& biometrics) const {
    const float corticalTone = clamp01(0.5f + meanOf(neuralWindow) * 0.15f);
    const float autonomicPenalty = clamp01(0.6f * biometrics.galvanicSkinResponse + 0.4f * (biometrics.heartRate / 180.0f));
    return clamp01(corticalTone - autonomicPenalty * 0.35f + biometrics.eyeFocus * 0.05f);
}

float CognitiveLoadPredictor::predictLoad(const std::vector<float>& neuralBandPower, const BiometricFrame& biometrics) const {
    const float bandMean = meanOf(neuralBandPower);
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
    frame.visualClarity = clamp01((1.0f - state.stress) * 0.45f + state.attention * 0.35f + (1.0f - state.fatigue) * 0.20f);
    frame.depthPrecision = clamp01(state.attention * 0.55f + (1.0f - state.fatigue) * 0.45f);
    frame.motionStability = clamp01((1.0f - state.stress) * 0.5f + biometrics.postureStability * 0.5f);
    frame.contrastEnhancement = clamp01(0.25f + state.immersionIndex * 0.5f + (1.0f - state.emotionValence) * 0.25f);
    const float visualHarmony = clamp01(frame.visualClarity * 0.30f + frame.depthPrecision * 0.25f + frame.motionStability * 0.25f + frame.contrastEnhancement * 0.20f);
    frame.shaderProfile = (state.stress > 0.70f) ? "stabilizing_calm" : ((visualHarmony > 0.78f) ? "deep_focus_hyperreal" : "balanced_dynamic");
    return frame;
}

std::vector<float> NeuralChipSignalAcquisition::ingestFrame(const std::vector<float>& rawFrame, const std::string& source) const {
    if (source.empty() || rawFrame.empty()) return {};
    std::vector<float> clamped;
    clamped.reserve(rawFrame.size());
    for (float sample : rawFrame) clamped.push_back(std::clamp(sample, -200.0f, 200.0f));
    return clamped;
}


float NeuralChipSignalAcquisition::estimateSignalQuality(const std::vector<float>& frame) const {
    if (frame.empty()) return 0.0f;
    float clipped = 0.0f;
    float meanAbs = 0.0f;
    for (const float s : frame) {
        if (std::abs(s) >= 199.5f) clipped += 1.0f;
        meanAbs += std::abs(s);
    }
    meanAbs /= static_cast<float>(frame.size());
    const float clipPenalty = clipped / static_cast<float>(frame.size());
    const float energyScore = std::clamp(meanAbs / 90.0f, 0.0f, 1.0f);
    return clamp01(0.7f * (1.0f - clipPenalty) + 0.3f * energyScore);
}

float NeuralChipSignalAcquisition::estimateVariance(const std::vector<float>& frame) const {
    return std::clamp(varianceOf(frame) / 4000.0f, 0.0f, 1.0f);
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

DeepImmersionPlatform::DeepImmersionPlatform() : historyLimit_(256) {
#if defined(DNXR_TARGET_OS_WINDOWS)
    platformProfile_ = RuntimePlatformProfile::Windows;
#elif defined(DNXR_TARGET_OS_LINUX)
    platformProfile_ = RuntimePlatformProfile::Linux;
#elif defined(DNXR_TARGET_OS_MACOS)
    platformProfile_ = RuntimePlatformProfile::MacOS;
#else
    platformProfile_ = RuntimePlatformProfile::Generic;
#endif
}

void DeepImmersionPlatform::appendHistory(const std::vector<float>& filteredFrame) {
    neuralHistory_.insert(neuralHistory_.end(), filteredFrame.begin(), filteredFrame.end());
    if (neuralHistory_.size() > historyLimit_) {
        const std::size_t trim = neuralHistory_.size() - historyLimit_;
        neuralHistory_.erase(neuralHistory_.begin(), neuralHistory_.begin() + static_cast<std::ptrdiff_t>(trim));
    }
}

std::vector<float> DeepImmersionPlatform::buildTemporalWindow(const std::vector<float>& filteredFrame) {
    appendHistory(filteredFrame);
    std::vector<float> window = neuralHistory_;
    if (window.size() < 16) {
        window.insert(window.end(), filteredFrame.begin(), filteredFrame.end());
    }
    return window;
}

CognitiveState DeepImmersionPlatform::runInferenceTick(const std::vector<float>& rawNeuralVectorFrame, const BiometricFrame& biometrics, const std::string& source) {
    // Entire inference path is vector-driven from neural chip streams (no EEG device dependency).
    const std::vector<float> acquired = acquisition_.ingestFrame(rawNeuralVectorFrame, source);
    const std::vector<float> filtered = denoise_.denoise(acquired);
    const float signalQuality = acquisition_.estimateSignalQuality(filtered);
    const float signalVariance = acquisition_.estimateVariance(filtered);
    const std::vector<float> temporal = buildTemporalWindow(filtered);
    const std::vector<float> bands = bandAnalyzer_.extractBands(temporal);

    CognitiveState state;
    state.motorIntent = intent_.decode(temporal);
    state.emotionValence = emotion_.inferValence(temporal, biometrics);
    state.fatigue = cognitiveLoad_.predictLoad(bands, biometrics);
    state.attention = clamp01(1.0f - state.fatigue * 0.65f + biometrics.eyeFocus * 0.25f);
    state.stress = clamp01((1.0f - state.emotionValence) * 0.45f + biometrics.galvanicSkinResponse * 0.55f);
    state.immersionIndex = clamp01((state.attention * 0.30f) + (state.motorIntent * 0.25f) + (state.emotionValence * 0.30f) + ((1.0f - state.stress) * 0.15f));

    // Safety fallback for low-confidence neural vectors to keep deep immersion stable and reversible.
    if (signalQuality < 0.25f || signalVariance < 0.01f) {
        state.stress = clamp01(state.stress + 0.20f);
        state.attention = clamp01(state.attention * 0.80f);
        state.immersionIndex = clamp01(state.immersionIndex * 0.65f);
    }

    lastDiagnostics_.streamSource = source;
    lastDiagnostics_.brainwaveClass = classifier_.classify(bands);
    lastDiagnostics_.neurofeedbackCue = neurofeedback_.buildFeedbackCue(state);
    lastDiagnostics_.safetyMode = (signalQuality < 0.25f || signalVariance < 0.01f) ? "stability_guard" : "nominal";
    lastDiagnostics_.biometricFusionScore = fusion_.fusionScore(state, biometrics);
    lastDiagnostics_.stressAdaptationGain = stressAdaptation_.adaptationGain(state.stress, biometrics.heartRate);
    lastDiagnostics_.attentionLock = attentionTracking_.attentionLock(state.attention, biometrics.eyeFocus, biometrics.postureStability);
    lastDiagnostics_.neuralSignalQuality = signalQuality;
    lastDiagnostics_.neuralSignalVariance = signalVariance;
    const XRAdaptationFrame visualFrame = adaptiveReality_.adapt(state, biometrics);
    lastDiagnostics_.visualHarmonyIndex = clamp01(visualFrame.visualClarity * 0.30f + visualFrame.depthPrecision * 0.25f + visualFrame.motionStability * 0.25f + visualFrame.contrastEnhancement * 0.20f);

    return state;
}

XRAdaptationFrame DeepImmersionPlatform::runAdaptationTick(const CognitiveState& state, const BiometricFrame& biometrics) const { return adaptiveReality_.adapt(state, biometrics); }

XRActionCommand DeepImmersionPlatform::runActionTick(const std::vector<float>& neuralVectorFrame, const CognitiveState& state) const {
    XRActionCommand cmd;
    if (neuralVectorFrame.empty()) return cmd;

    const std::size_t n = neuralVectorFrame.size();
    const float left = std::abs(neuralVectorFrame[0 % n]);
    const float right = std::abs(neuralVectorFrame[1 % n]);
    const float forward = std::abs(neuralVectorFrame[2 % n]);
    const float backward = std::abs(neuralVectorFrame[3 % n]);
    const float yawL = std::abs(neuralVectorFrame[4 % n]);
    const float yawR = std::abs(neuralVectorFrame[5 % n]);
    const float pitchU = std::abs(neuralVectorFrame[6 % n]);
    const float pitchD = std::abs(neuralVectorFrame[7 % n]);

    const float confidence = clamp01(state.motorIntent * 0.65f + state.attention * 0.35f);
    const float safetyLimiter = (lastDiagnostics_.safetyMode == "stability_guard") ? 0.45f : 1.0f;

    cmd.moveX = std::clamp((right - left) * 0.02f * confidence * safetyLimiter, -1.0f, 1.0f);
    cmd.moveY = std::clamp((forward - backward) * 0.02f * confidence * safetyLimiter, -1.0f, 1.0f);
    cmd.lookYaw = std::clamp((yawR - yawL) * 0.015f * confidence * safetyLimiter, -1.0f, 1.0f);
    cmd.lookPitch = std::clamp((pitchU - pitchD) * 0.015f * confidence * safetyLimiter, -1.0f, 1.0f);
    cmd.interactStrength = clamp01((state.motorIntent * 0.6f + state.immersionIndex * 0.4f) * safetyLimiter);
    cmd.jump = confidence > 0.64f && state.stress < 0.7f && forward > backward * 1.2f;
    cmd.grab = cmd.interactStrength > 0.55f && (right + left) > 0.35f;
    return cmd;
}



XRFullBodyCommand DeepImmersionPlatform::runFullBodyControlTick(const std::vector<float>& neuralVectorFrame, const CognitiveState& state) {
    XRFullBodyCommand body;
    if (neuralVectorFrame.empty()) return body;

    const std::size_t n = neuralVectorFrame.size();
    auto sample = [&](std::size_t i) { return std::abs(neuralVectorFrame[i % n]); };

    const float motorPrecision = clamp01(state.motorIntent * 0.7f + state.attention * 0.3f);
    const float safetyLimiter = (lastDiagnostics_.safetyMode == "stability_guard") ? 0.50f : 1.0f;
    float platformGain = 1.0f;
    if (platformProfile_ == RuntimePlatformProfile::Windows) platformGain = 1.02f;
    else if (platformProfile_ == RuntimePlatformProfile::Linux) platformGain = 1.00f;
    else if (platformProfile_ == RuntimePlatformProfile::MacOS) platformGain = 0.98f;

    const float gain = motorPrecision * safetyLimiter * platformGain;

    body.headYaw = std::clamp((sample(9) - sample(10)) * 0.02f * gain, -1.0f, 1.0f);
    body.headPitch = std::clamp((sample(11) - sample(12)) * 0.02f * gain, -1.0f, 1.0f);
    body.torsoYaw = std::clamp((sample(13) - sample(14)) * 0.018f * gain, -1.0f, 1.0f);
    body.torsoLean = std::clamp((sample(15) - sample(16)) * 0.018f * gain, -1.0f, 1.0f);

    body.leftArmLift = clamp01(sample(17) * 0.03f * gain);
    body.rightArmLift = clamp01(sample(18) * 0.03f * gain);
    body.leftHandGrip = clamp01(sample(19) * 0.04f * gain);
    body.rightHandGrip = clamp01(sample(20) * 0.04f * gain);

    body.leftLegStride = std::clamp((sample(21) - sample(22)) * 0.02f * gain, -1.0f, 1.0f);
    body.rightLegStride = std::clamp((sample(23) - sample(24)) * 0.02f * gain, -1.0f, 1.0f);
    body.locomotionForward = std::clamp((sample(2) - sample(3)) * 0.02f * gain, -1.0f, 1.0f);
    body.locomotionLateral = std::clamp((sample(1) - sample(0)) * 0.02f * gain, -1.0f, 1.0f);

    const float posturalRisk = clamp01(state.stress * 0.6f + (1.0f - state.attention) * 0.4f);
    body.balanceAssist = clamp01((1.0f - motorPrecision) * 0.5f + posturalRisk * 0.5f);
    return smoothFullBodyCommand(body);
}



XRFullBodyCommand DeepImmersionPlatform::smoothFullBodyCommand(const XRFullBodyCommand& current) {
    const float alpha = 0.35f;
    XRFullBodyCommand out = current;
    auto blend = [alpha](float prev, float cur) { return prev * (1.0f - alpha) + cur * alpha; };

    out.headYaw = blend(lastBodyCommand_.headYaw, current.headYaw);
    out.headPitch = blend(lastBodyCommand_.headPitch, current.headPitch);
    out.torsoYaw = blend(lastBodyCommand_.torsoYaw, current.torsoYaw);
    out.torsoLean = blend(lastBodyCommand_.torsoLean, current.torsoLean);
    out.leftArmLift = blend(lastBodyCommand_.leftArmLift, current.leftArmLift);
    out.rightArmLift = blend(lastBodyCommand_.rightArmLift, current.rightArmLift);
    out.leftHandGrip = blend(lastBodyCommand_.leftHandGrip, current.leftHandGrip);
    out.rightHandGrip = blend(lastBodyCommand_.rightHandGrip, current.rightHandGrip);
    out.leftLegStride = blend(lastBodyCommand_.leftLegStride, current.leftLegStride);
    out.rightLegStride = blend(lastBodyCommand_.rightLegStride, current.rightLegStride);
    out.locomotionForward = blend(lastBodyCommand_.locomotionForward, current.locomotionForward);
    out.locomotionLateral = blend(lastBodyCommand_.locomotionLateral, current.locomotionLateral);
    out.balanceAssist = blend(lastBodyCommand_.balanceAssist, current.balanceAssist);

    lastBodyCommand_ = out;
    return out;
}

void DeepImmersionPlatform::setRuntimePlatformProfile(RuntimePlatformProfile profile) { platformProfile_ = profile; }
RuntimePlatformProfile DeepImmersionPlatform::runtimePlatformProfile() const { return platformProfile_; }

NeuroPipelineDiagnostics DeepImmersionPlatform::lastDiagnostics() const { return lastDiagnostics_; }

} // namespace dnxr
