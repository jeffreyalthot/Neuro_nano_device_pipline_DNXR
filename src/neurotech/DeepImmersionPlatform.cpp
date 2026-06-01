// =============================================================================
// DOSSIER : src/neurotech/
// FICHIER : DeepImmersionPlatform.cpp
//
// AMÉLIORATIONS APPLIQUÉES :
//   [1] NeuralIntentDecoder::decode — remplacé énergie brute par scoring
//       multivarié (énergie + variance + pattern temporel) avec seuil de
//       confiance minimal (kMinIntentConfidence = 0.65f). Toute intention
//       sous ce seuil est bloquée → motorIntent = 0.
//   [2] historyLimit_ adaptatif — la fenêtre temporelle s'élargit à 512
//       en état "fatigued" pour stabiliser l'inférence sur sessions longues.
//   [3] NeuralChipSignalAcquisition::ingestFrame — plage normalisée [-1,1]
//       alignée avec SignalStreamProcessor pour supprimer le double-clamp
//       incohérent. estimateSignalQuality recalibrée en conséquence.
//   [4] estimateVariance — dénominateur ajusté à la nouvelle plage [-1,1].
// =============================================================================

#include "DeepImmersionPlatform.h"

#include "../safety/IntentRiskClassifier.h"

#include <algorithm>
#include <numeric>
#include <cmath>
#include <chrono>

namespace dnxr {

namespace {

// Seuil de confiance minimal pour qu'une intention neurale soit acceptée.
// Sous ce seuil, motorIntent est forcé à 0 pour éviter les faux positifs.
constexpr float kMinIntentConfidence = 0.65f;

// Plage normalisée unifiée pour l'ensemble du pipeline neural chip.
// Alignée avec SignalStreamProcessor::safeClamp ([-1, 1]).
constexpr float kNeuralSignalMin = -1.0f;
constexpr float kNeuralSignalMax =  1.0f;

// Valeur de normalisation de l'énergie pour estimateSignalQuality.
// Avec des signaux en [-1,1], l'énergie moyenne maximale est ~0.5
// (distribution uniforme). On normalise sur 0.6 pour garder une
// marge et éviter un score artificiellement saturé.
constexpr float kEnergyNormFactor = 0.6f;

// Seuil de clip pour la nouvelle plage : un sample est "clippé" s'il
// atteint ±0.995 (soit 99.5% de la plage max).
constexpr float kClipThreshold = 0.995f;

bool tokenAllowsMove(const std::string& token) {
    return token == "xr.move" || token == "xr.compose";
}

bool tokenAllowsLook(const std::string& token) {
    return token == "xr.look" || token == "xr.rotate" || token == "xr.compose";
}

bool tokenAllowsInteract(const std::string& token) {
    return token == "xr.grab" || token == "xr.compose";
}

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

// ─────────────────────────────────────────────────────────────────────────────
// [AMÉLIORATION 1] Scoring d'intention multivarié
// Remplace l'énergie absolue brute par une combinaison pondérée de :
//   • énergie normalisée  (proxy de l'intensité du signal)
//   • variance normalisée (proxy de la richesse du pattern)
//   • cohérence temporelle (stabilité entre première et seconde moitié)
// Retourne un score en [0,1] ET un score de confiance séparé.
// ─────────────────────────────────────────────────────────────────────────────
struct IntentScoreResult {
    float score;
    float confidence;
};

IntentScoreResult computeIntentScore(const std::vector<float>& neuralWindow) {
    if (neuralWindow.empty()) return {0.0f, 0.0f};

    const std::size_t n = neuralWindow.size();

    // — Énergie normalisée (signaux en [-1,1] → max énergie abs ~1.0)
    float absEnergy = 0.0f;
    for (float s : neuralWindow) absEnergy += std::abs(s);
    const float normEnergy = clamp01(absEnergy / (static_cast<float>(n) * 0.6f));

    // — Variance normalisée (sur plage [-1,1], variance max théorique ≈ 0.33)
    const float var = varianceOf(neuralWindow);
    const float normVariance = clamp01(var / 0.33f);

    // — Cohérence temporelle : corrélation de signe entre 1ère et 2ème moitié.
    //   Si les deux moitiés oscillent dans le même sens → signal structuré.
    float coherence = 0.0f;
    if (n >= 4) {
        const std::size_t half = n / 2;
        float sumFirst = 0.0f, sumSecond = 0.0f;
        for (std::size_t i = 0; i < half; ++i)       sumFirst  += neuralWindow[i];
        for (std::size_t i = half; i < n; ++i)        sumSecond += neuralWindow[i];
        // Même signe → cohérence = 1.0, signes opposés → 0.0
        coherence = (sumFirst * sumSecond > 0.0f) ? 1.0f : 0.0f;
    }

    // Score d'intention composite
    const float score = clamp01(
        normEnergy  * 0.50f +
        normVariance * 0.30f +
        coherence   * 0.20f
    );

    // Confiance : élevée si énergie ET variance concordent, faible sinon
    const float confidence = clamp01(
        (normEnergy > 0.30f ? 0.5f : 0.0f) +
        (normVariance > 0.10f ? 0.3f : 0.0f) +
        coherence * 0.2f
    );

    return {score, confidence};
}

} // namespace

// ─────────────────────────────────────────────────────────────────────────────
// [AMÉLIORATION 1] NeuralIntentDecoder::decode
// Utilise computeIntentScore et bloque toute intention sous kMinIntentConfidence.
// ─────────────────────────────────────────────────────────────────────────────
float NeuralIntentDecoder::decode(const std::vector<float>& neuralWindow) const {
    if (neuralWindow.empty()) return 0.0f;

    const IntentScoreResult result = computeIntentScore(neuralWindow);

    // Blocage de sécurité : intention rejetée si confiance insuffisante.
    // Prévient les faux positifs dus à des artefacts musculaires ou au bruit.
    if (result.confidence < kMinIntentConfidence) {
        return 0.0f;
    }

    return result.score;
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
    frame.sceneIntensity      = clamp01(0.35f + state.immersionIndex * 0.65f - state.fatigue * 0.25f);
    frame.musicArousal        = clamp01(0.25f + state.attention * 0.30f + state.emotionValence * 0.45f);
    frame.hapticsDensity      = clamp01(state.motorIntent * 0.65f + biometrics.movementEnergy * 0.35f);
    frame.narrativePressure   = clamp01(state.stress * 0.65f + state.attention * 0.35f);
    frame.distortionAmount    = clamp01(state.stress * 0.7f + state.fatigue * 0.3f);
    frame.visualClarity       = clamp01((1.0f - state.stress) * 0.45f + state.attention * 0.35f + (1.0f - state.fatigue) * 0.20f);
    frame.depthPrecision      = clamp01(state.attention * 0.55f + (1.0f - state.fatigue) * 0.45f);
    frame.motionStability     = clamp01((1.0f - state.stress) * 0.5f + biometrics.postureStability * 0.5f);
    frame.contrastEnhancement = clamp01(0.25f + state.immersionIndex * 0.5f + (1.0f - state.emotionValence) * 0.25f);

    const float visualHarmony = clamp01(
        frame.visualClarity       * 0.30f +
        frame.depthPrecision      * 0.25f +
        frame.motionStability     * 0.25f +
        frame.contrastEnhancement * 0.20f
    );

    frame.shaderProfile = (state.stress > 0.70f)
        ? "stabilizing_calm"
        : ((visualHarmony > 0.78f) ? "deep_focus_hyperreal" : "balanced_dynamic");

    return frame;
}

// ─────────────────────────────────────────────────────────────────────────────
// [AMÉLIORATION 3] NeuralChipSignalAcquisition::ingestFrame
// Plage normalisée unifiée [-1, 1] alignée avec SignalStreamProcessor.
// L'ancien clamp [-200, 200] créait une incohérence : les signaux déjà
// normalisés par SignalStreamProcessor ([-1,1] via tanh) n'atteignaient
// jamais ±200, rendant clipPenalty toujours 0 et faussant estimateSignalQuality.
// ─────────────────────────────────────────────────────────────────────────────
std::vector<float> NeuralChipSignalAcquisition::ingestFrame(const std::vector<float>& rawFrame, const std::string& source) const {
    if (source.empty() || rawFrame.empty()) return {};

    std::vector<float> clamped;
    clamped.reserve(rawFrame.size());
    for (float sample : rawFrame) {
        clamped.push_back(std::clamp(sample, kNeuralSignalMin, kNeuralSignalMax));
    }
    return clamped;
}

// ─────────────────────────────────────────────────────────────────────────────
// [AMÉLIORATION 3] NeuralChipSignalAcquisition::estimateSignalQuality
// Recalibrée pour la plage [-1, 1] :
//   • clipPenalty : proportion de samples à ±0.995 (au lieu de ±199.5)
//   • energyScore : normalisé sur kEnergyNormFactor (0.6) au lieu de 90.0
// La qualité est désormais significative et détecte réellement les artefacts.
// ─────────────────────────────────────────────────────────────────────────────
float NeuralChipSignalAcquisition::estimateSignalQuality(const std::vector<float>& frame) const {
    if (frame.empty()) return 0.0f;

    float clipped = 0.0f;
    float meanAbs = 0.0f;

    for (const float s : frame) {
        if (std::abs(s) >= kClipThreshold) clipped += 1.0f;
        meanAbs += std::abs(s);
    }

    meanAbs /= static_cast<float>(frame.size());
    const float clipPenalty = clipped / static_cast<float>(frame.size());
    const float energyScore = std::clamp(meanAbs / kEnergyNormFactor, 0.0f, 1.0f);

    return clamp01(0.7f * (1.0f - clipPenalty) + 0.3f * energyScore);
}

// ─────────────────────────────────────────────────────────────────────────────
// [AMÉLIORATION 4] NeuralChipSignalAcquisition::estimateVariance
// Dénominateur ajusté à la plage [-1, 1] :
//   • Variance maximale théorique sur [-1,1] ≈ 0.33 (distribution uniforme).
//   • On normalise sur 0.35 pour conserver une marge et éviter la saturation.
//   • L'ancien dénominateur 4000.0 était calibré pour [-200,200] et donnait
//     des valeurs proches de 0 pour des signaux normalisés → fallback sécurité
//     se déclenchait quasi-systématiquement (signalVariance < 0.01).
// ─────────────────────────────────────────────────────────────────────────────
float NeuralChipSignalAcquisition::estimateVariance(const std::vector<float>& frame) const {
    return std::clamp(varianceOf(frame) / 0.35f, 0.0f, 1.0f);
}

std::vector<float> NoiseReductionPipeline::denoise(const std::vector<float>& frame) const {
    if (frame.size() < 5) return frame;
    std::vector<float> out(frame.size(), 0.0f);
    out[0] = frame[0]; out[1] = frame[1];
    out[frame.size() - 2] = frame[frame.size() - 2];
    out.back() = frame.back();
    for (std::size_t i = 2; i + 2 < frame.size(); ++i) {
        out[i] = (frame[i-2] + 2.0f*frame[i-1] + 3.0f*frame[i] + 2.0f*frame[i+1] + frame[i+2]) / 9.0f;
    }
    return out;
}

std::vector<float> FrequencyBandAnalyzer::extractBands(const std::vector<float>& frame) const {
    if (frame.empty()) return {};
    float low = 0, mid = 0, high = 0;
    for (std::size_t i = 0; i < frame.size(); ++i) {
        const float a = std::abs(frame[i]);
        if      (i % 3 == 0) low  += a;
        else if (i % 3 == 1) mid  += a;
        else                  high += a;
    }
    const float norm = std::max(1.0f, static_cast<float>(frame.size()) * 20.0f);
    return {
        clamp01(low / norm),
        clamp01((low + mid * 0.5f) / norm),
        clamp01(mid / norm),
        clamp01((mid + high * 0.5f) / norm),
        clamp01(high / norm)
    };
}

std::string BrainwaveClassifier::classify(const std::vector<float>& bandPower) const {
    if (bandPower.size() < 5) return "unknown";
    const float alphaBeta  = bandPower[2] + bandPower[3];
    const float thetaDelta = bandPower[0] + bandPower[1];
    if (alphaBeta  > thetaDelta * 1.4f) return "focused";
    if (thetaDelta > alphaBeta  * 1.5f) return "fatigued";
    if (bandPower[4] > 0.45f)           return "high_arousal";
    return "neutral";
}

std::string NeurofeedbackController::buildFeedbackCue(const CognitiveState& state) const {
    if (state.stress         > 0.75f) return "breathe_and_reduce_fx";
    if (state.fatigue        > 0.72f) return "micro_rest_window";
    if (state.attention      < 0.38f) return "anchor_attention_target";
    if (state.immersionIndex > 0.80f) return "maintain_deep_dive";
    return "adaptive_balance";
}

float BiometricFusionEngine::fusionScore(const CognitiveState& state, const BiometricFrame& b) const {
    const float phys = clamp01((b.heartRate / 190.0f + b.eyeFocus + b.postureStability + (1.0f - b.galvanicSkinResponse)) / 4.0f);
    return clamp01(phys * 0.5f + state.immersionIndex * 0.5f);
}

float StressAdaptationModel::adaptationGain(float stress, float heartRate) const {
    return clamp01(stress * 0.7f + (heartRate / 210.0f) * 0.3f);
}

float AttentionTrackingSystem::attentionLock(float attention, float eyeFocus, float postureStability) const {
    return clamp01(attention * 0.45f + eyeFocus * 0.35f + postureStability * 0.2f);
}

// ─────────────────────────────────────────────────────────────────────────────
// [AMÉLIORATION 2] Constructeur — historyLimit_ adaptatif
// La valeur par défaut reste 256 (état normal).
// Elle sera ajustée dynamiquement dans runInferenceTick selon brainwaveClass.
// ─────────────────────────────────────────────────────────────────────────────
DeepImmersionPlatform::DeepImmersionPlatform() : historyLimit_(256) {
    consentManager_.setPolicy({true, true, true, "default_runtime_policy"});

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
        neuralHistory_.erase(neuralHistory_.begin(),
                             neuralHistory_.begin() + static_cast<std::ptrdiff_t>(trim));
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

// ─────────────────────────────────────────────────────────────────────────────
// [AMÉLIORATION 2] runInferenceTick — historyLimit_ adaptatif par brainwaveClass
// "fatigued"    → 512 (fenêtre plus large pour lisser la dérive de signal)
// "high_arousal"→ 128 (fenêtre courte, réactivité maximale)
// "focused"     → 256 (nominal)
// "neutral"     → 256 (nominal)
// L'ajustement se fait AVANT appendHistory() pour que la frame courante
// soit déjà stockée dans le bon contexte.
// ─────────────────────────────────────────────────────────────────────────────
CognitiveState DeepImmersionPlatform::runInferenceTick(
    const std::vector<float>& rawNeuralVectorFrame,
    const BiometricFrame& biometrics,
    const std::string& source)
{
    // Entire inference path is vector-driven from neural chip streams
    // (no EEG device dependency).
    InfluenceTrace influence{};
    const std::vector<float> influenced  = influenceEngine_.applyInfluence(rawNeuralVectorFrame, inducedVector_, influence);

    // [FIX 3] ingestFrame normalise maintenant en [-1,1] comme SignalStreamProcessor.
    // Pas de double-clamp incohérent : les deux étages partagent la même plage.
    const std::vector<float> acquired    = acquisition_.ingestFrame(influenced, source);
    const std::vector<float> filtered    = denoise_.denoise(acquired);
    const float signalQuality            = acquisition_.estimateSignalQuality(filtered);
    const float signalVariance           = acquisition_.estimateVariance(filtered);

    // [AMÉLIORATION 2] Classifier les bandes AVANT appendHistory pour adapter
    // historyLimit_ en fonction de l'état cognitif détecté.
    const std::vector<float> bandsEarly  = bandAnalyzer_.extractBands(filtered);
    const std::string earlyBrainwave     = classifier_.classify(bandsEarly);

    if      (earlyBrainwave == "fatigued")     historyLimit_ = 512u;
    else if (earlyBrainwave == "high_arousal") historyLimit_ = 128u;
    else                                        historyLimit_ = 256u;

    const std::vector<float> temporal    = buildTemporalWindow(filtered);
    const std::vector<float> bands       = bandAnalyzer_.extractBands(temporal);

    const bool consentIntent = consentManager_.isAllowed("intent");
    const bool consentBio    = consentManager_.isAllowed("biometric");

    CognitiveState state;

    // [AMÉLIORATION 1] motorIntent filtre les intentions sous seuil de confiance.
    // NeuralIntentDecoder::decode retourne 0 si confidence < kMinIntentConfidence.
    state.motorIntent    = consentIntent ? intent_.decode(temporal) : 0.0f;
    state.emotionValence = emotion_.inferValence(temporal, biometrics);
    state.fatigue        = consentBio
                           ? cognitiveLoad_.predictLoad(bands, biometrics)
                           : clamp01(meanOf(bands));
    state.attention      = clamp01(1.0f - state.fatigue * 0.65f + biometrics.eyeFocus * 0.25f);
    state.stress         = clamp01((1.0f - state.emotionValence) * 0.45f + biometrics.galvanicSkinResponse * 0.55f);
    state.immersionIndex = clamp01(
        (state.attention    * 0.30f) +
        (state.motorIntent  * 0.25f) +
        (state.emotionValence* 0.30f) +
        ((1.0f - state.stress) * 0.15f)
    );

    // Safety fallback for low-confidence neural vectors to keep deep immersion
    // stable and reversible.
    if (signalQuality < 0.25f || signalVariance < 0.01f) {
        state.stress         = clamp01(state.stress + 0.20f);
        state.attention      = clamp01(state.attention * 0.80f);
        state.immersionIndex = clamp01(state.immersionIndex * 0.65f);
    }

    lastDiagnostics_.streamSource          = source;
    lastDiagnostics_.brainwaveClass        = classifier_.classify(bands);
    lastDiagnostics_.neurofeedbackCue      = neurofeedback_.buildFeedbackCue(state);
    lastDiagnostics_.safetyMode            = (signalQuality < 0.25f || signalVariance < 0.01f)
                                             ? "stability_guard" : "nominal";
    lastDiagnostics_.biometricFusionScore  = consentBio ? fusion_.fusionScore(state, biometrics) : 0.0f;
    lastDiagnostics_.stressAdaptationGain  = stressAdaptation_.adaptationGain(state.stress, biometrics.heartRate);
    lastDiagnostics_.attentionLock         = attentionTracking_.attentionLock(state.attention, biometrics.eyeFocus, biometrics.postureStability);
    lastDiagnostics_.neuralSignalQuality   = signalQuality;
    lastDiagnostics_.neuralSignalVariance  = signalVariance;
    lastDiagnostics_.inducedInfluence      = influence.inducedInfluence;
    lastDiagnostics_.decisionDrift         = influence.decisionDrift;
    lastDiagnostics_.autonomyPreservation  = influence.autonomyPreservation;

    const float baseConfidence = clamp01(state.motorIntent * 0.6f + state.attention * 0.4f);
    const NeuralIntentVector intentVector  = intentProtocol_.decode(temporal, baseConfidence);
    lastDiagnostics_.intentConfidence            = intentVector.confidence;
    lastDiagnostics_.intentSchemaVersion         = intentVector.schemaVersion;
    lastDiagnostics_.intentActionToken           = intentVector.actionToken;
    lastDiagnostics_.intentAmbiguityMargin       = intentVector.ambiguityMargin;
    lastDiagnostics_.intentAmbiguityDetected     = intentVector.ambiguityDetected;
    lastDiagnostics_.intentFallbackRecommended   = intentVector.fallbackRecommended;

    const bool confirmationEligible = intentVector.actionToken != "none" &&
                                      !intentVector.fallbackRecommended &&
                                      intentVector.confidence >= 0.45f;
    if (confirmationEligible && intentVector.actionToken == lastIntentToken_) {
        ++intentConfirmationCount_;
    } else if (confirmationEligible) {
        lastIntentToken_ = intentVector.actionToken;
        intentConfirmationCount_ = 1;
    } else {
        lastIntentToken_ = "none";
        intentConfirmationCount_ = 0;
    }

    const IntentRiskDecision intentRisk = IntentRiskClassifier{}.evaluate(IntentRiskInput{
        intentVector.actionToken,
        intentVector.confidence,
        intentVector.ambiguityMargin,
        intentVector.ambiguityDetected,
        intentVector.fallbackRecommended,
        intentConfirmationCount_,
        signalQuality,
        influence.autonomyPreservation,
        state.stress,
        state.fatigue,
        consentManager_.isAllowed("intent")});

    const bool executionAuthorized = confirmationEligible && intentRisk.authorized;
    lastDiagnostics_.intentConfirmationCount = intentConfirmationCount_;
    lastDiagnostics_.intentRiskLevel = intentRisk.riskLevel;
    lastDiagnostics_.intentRiskReason = intentRisk.reason;
    lastDiagnostics_.intentExecutionAuthorized = executionAuthorized;
    lastDiagnostics_.authorizedIntentToken = executionAuthorized ? intentVector.actionToken : "none";

    const float jitterComp = clamp01(signalVariance * 0.5f + (1.0f - signalQuality) * 0.5f);
    lastDiagnostics_.jitterCompensation    = jitterComp;

    if (intentRisk.requiresRealityAnchor) {
        lastDiagnostics_.fallbackChannel   = "reality_anchor_consent";
        lastDiagnostics_.safetyMode        = intentRisk.safetyMode;
        state.immersionIndex               = clamp01(state.immersionIndex * 0.55f);
    } else if (intentRisk.requiresFallback || intentVector.fallbackRecommended || intentVector.confidence < 0.45f) {
        lastDiagnostics_.fallbackChannel   = intentVector.ambiguityDetected
                                             ? "voice_gaze_gesture_ambiguity"
                                             : "voice_gaze_gesture";
        lastDiagnostics_.safetyMode        = intentRisk.safetyMode == "nominal" ? "degraded_fallback" : intentRisk.safetyMode;
        state.immersionIndex               = clamp01(state.immersionIndex * 0.75f);
    } else if (!executionAuthorized) {
        lastDiagnostics_.fallbackChannel   = "intent_confirmation_hold";
        lastDiagnostics_.safetyMode        = "intent_confirmation_hold";
        state.immersionIndex               = clamp01(state.immersionIndex * 0.85f);
    } else {
        lastDiagnostics_.fallbackChannel   = "neural_primary";
    }

    const XRAdaptationFrame visualFrame    = adaptiveReality_.adapt(state, biometrics);
    lastDiagnostics_.visualHarmonyIndex    = clamp01(
        visualFrame.visualClarity       * 0.30f +
        visualFrame.depthPrecision      * 0.25f +
        visualFrame.motionStability     * 0.25f +
        visualFrame.contrastEnhancement * 0.20f
    );

    const float sceneInstability = clamp01(1.0f - visualFrame.motionStability + visualFrame.distortionAmount * 0.6f);
    const AddictionRiskReport risk = addictionGuard_.evaluate(
        sceneInstability, state.emotionValence, state.stress, 30.0f);
    lastDiagnostics_.addictionRisk              = risk.addictionRisk;
    lastDiagnostics_.addictionMitigationActive  = risk.mitigationActive;
    lastDiagnostics_.addictionPolicyAction       = risk.policyAction;

    if (risk.mitigationActive) {
        lastDiagnostics_.safetyMode  = "addiction_risk_mitigation";
        state.immersionIndex         = clamp01(state.immersionIndex * 0.70f);
    }

    return state;
}

XRAdaptationFrame DeepImmersionPlatform::runAdaptationTick(const CognitiveState& state, const BiometricFrame& biometrics) const {
    return adaptiveReality_.adapt(state, biometrics);
}

XRActionCommand DeepImmersionPlatform::runActionTick(const std::vector<float>& neuralVectorFrame, const CognitiveState& state) const {
    XRActionCommand cmd;
    if (neuralVectorFrame.empty()) return cmd;

    const std::size_t n = neuralVectorFrame.size();
    const float left     = std::abs(neuralVectorFrame[0 % n]);
    const float right    = std::abs(neuralVectorFrame[1 % n]);
    const float forward  = std::abs(neuralVectorFrame[2 % n]);
    const float backward = std::abs(neuralVectorFrame[3 % n]);
    const float yawL     = std::abs(neuralVectorFrame[4 % n]);
    const float yawR     = std::abs(neuralVectorFrame[5 % n]);
    const float pitchU   = std::abs(neuralVectorFrame[6 % n]);
    const float pitchD   = std::abs(neuralVectorFrame[7 % n]);

    const float confidence = clamp01(state.motorIntent * 0.65f + state.attention * 0.35f);
    float safetyLimiter = 1.0f;
    if (lastDiagnostics_.safetyMode == "stability_guard") safetyLimiter = 0.45f;
    else if (lastDiagnostics_.safetyMode == "degraded_fallback") safetyLimiter = 0.35f;
    else if (lastDiagnostics_.safetyMode == "intent_confirmation_hold") safetyLimiter = 0.25f;
    else if (lastDiagnostics_.safetyMode == "addiction_risk_mitigation") safetyLimiter = 0.30f;

    const std::string& authorizedToken = lastDiagnostics_.authorizedIntentToken;
    const bool allowMove = lastDiagnostics_.intentExecutionAuthorized && tokenAllowsMove(authorizedToken);
    const bool allowLook = lastDiagnostics_.intentExecutionAuthorized && tokenAllowsLook(authorizedToken);
    const bool allowInteract = lastDiagnostics_.intentExecutionAuthorized && tokenAllowsInteract(authorizedToken);

    cmd.moveX           = allowMove ? std::clamp((right - left)     * 0.02f * confidence * safetyLimiter, -1.0f, 1.0f) : 0.0f;
    cmd.moveY           = allowMove ? std::clamp((forward - backward)* 0.02f * confidence * safetyLimiter, -1.0f, 1.0f) : 0.0f;
    cmd.lookYaw         = allowLook ? std::clamp((yawR - yawL)      * 0.015f * confidence * safetyLimiter, -1.0f, 1.0f) : 0.0f;
    cmd.lookPitch       = allowLook ? std::clamp((pitchU - pitchD)  * 0.015f * confidence * safetyLimiter, -1.0f, 1.0f) : 0.0f;
    cmd.interactStrength= allowInteract ? clamp01((state.motorIntent * 0.6f + state.immersionIndex * 0.4f) * safetyLimiter) : 0.0f;
    cmd.jump            = allowMove && confidence > 0.64f && state.stress < 0.7f && forward > backward * 1.2f;
    cmd.grab            = allowInteract && cmd.interactStrength > 0.55f && (right + left) > 0.35f;

    return cmd;
}

XRFullBodyCommand DeepImmersionPlatform::runFullBodyControlTick(const std::vector<float>& neuralVectorFrame, const CognitiveState& state) {
    XRFullBodyCommand body;
    if (neuralVectorFrame.empty()) return body;

    const std::size_t n = neuralVectorFrame.size();
    auto sample = [&](std::size_t i) { return std::abs(neuralVectorFrame[i % n]); };

    const float motorPrecision = clamp01(state.motorIntent * 0.7f + state.attention * 0.3f);
    float safetyLimiter = 1.0f;
    if (lastDiagnostics_.safetyMode == "stability_guard") safetyLimiter = 0.50f;
    else if (lastDiagnostics_.safetyMode == "degraded_fallback") safetyLimiter = 0.40f;
    else if (lastDiagnostics_.safetyMode == "intent_confirmation_hold") safetyLimiter = 0.30f;
    else if (lastDiagnostics_.safetyMode == "addiction_risk_mitigation") safetyLimiter = 0.35f;

    float platformGain = 1.0f;
    if      (platformProfile_ == RuntimePlatformProfile::Windows) platformGain = 1.02f;
    else if (platformProfile_ == RuntimePlatformProfile::Linux)   platformGain = 1.00f;
    else if (platformProfile_ == RuntimePlatformProfile::MacOS)   platformGain = 0.98f;

    const float gain = motorPrecision * safetyLimiter * platformGain;
    const std::string& authorizedToken = lastDiagnostics_.authorizedIntentToken;
    const bool allowMove = lastDiagnostics_.intentExecutionAuthorized && tokenAllowsMove(authorizedToken);
    const bool allowLook = lastDiagnostics_.intentExecutionAuthorized && tokenAllowsLook(authorizedToken);
    const bool allowInteract = lastDiagnostics_.intentExecutionAuthorized && tokenAllowsInteract(authorizedToken);

    body.headYaw          = allowLook ? std::clamp((sample(9)  - sample(10)) * 0.02f  * gain, -1.0f, 1.0f) : 0.0f;
    body.headPitch        = allowLook ? std::clamp((sample(11) - sample(12)) * 0.02f  * gain, -1.0f, 1.0f) : 0.0f;
    body.torsoYaw         = allowLook ? std::clamp((sample(13) - sample(14)) * 0.018f * gain, -1.0f, 1.0f) : 0.0f;
    body.torsoLean        = allowMove ? std::clamp((sample(15) - sample(16)) * 0.018f * gain, -1.0f, 1.0f) : 0.0f;
    body.leftArmLift      = allowInteract ? clamp01(sample(17) * 0.03f * gain) : 0.0f;
    body.rightArmLift     = allowInteract ? clamp01(sample(18) * 0.03f * gain) : 0.0f;
    body.leftHandGrip     = allowInteract ? clamp01(sample(19) * 0.04f * gain) : 0.0f;
    body.rightHandGrip    = allowInteract ? clamp01(sample(20) * 0.04f * gain) : 0.0f;
    body.leftLegStride    = allowMove ? std::clamp((sample(21) - sample(22)) * 0.02f * gain, -1.0f, 1.0f) : 0.0f;
    body.rightLegStride   = allowMove ? std::clamp((sample(23) - sample(24)) * 0.02f * gain, -1.0f, 1.0f) : 0.0f;
    body.locomotionForward= allowMove ? std::clamp((sample(2)  - sample(3))  * 0.02f * gain, -1.0f, 1.0f) : 0.0f;
    body.locomotionLateral= allowMove ? std::clamp((sample(1)  - sample(0))  * 0.02f * gain, -1.0f, 1.0f) : 0.0f;

    const float posturalRisk = clamp01(state.stress * 0.6f + (1.0f - state.attention) * 0.4f);
    body.balanceAssist = clamp01((1.0f - motorPrecision) * 0.5f + posturalRisk * 0.5f);

    return smoothFullBodyCommand(body);
}

XRFullBodyCommand DeepImmersionPlatform::smoothFullBodyCommand(const XRFullBodyCommand& current) {
    constexpr float alpha = 0.35f;
    XRFullBodyCommand out = current;
    auto blend = [alpha](float prev, float cur) { return prev * (1.0f - alpha) + cur * alpha; };

    out.headYaw           = blend(lastBodyCommand_.headYaw,           current.headYaw);
    out.headPitch         = blend(lastBodyCommand_.headPitch,         current.headPitch);
    out.torsoYaw          = blend(lastBodyCommand_.torsoYaw,          current.torsoYaw);
    out.torsoLean         = blend(lastBodyCommand_.torsoLean,         current.torsoLean);
    out.leftArmLift       = blend(lastBodyCommand_.leftArmLift,       current.leftArmLift);
    out.rightArmLift      = blend(lastBodyCommand_.rightArmLift,      current.rightArmLift);
    out.leftHandGrip      = blend(lastBodyCommand_.leftHandGrip,      current.leftHandGrip);
    out.rightHandGrip     = blend(lastBodyCommand_.rightHandGrip,     current.rightHandGrip);
    out.leftLegStride     = blend(lastBodyCommand_.leftLegStride,     current.leftLegStride);
    out.rightLegStride    = blend(lastBodyCommand_.rightLegStride,    current.rightLegStride);
    out.locomotionForward = blend(lastBodyCommand_.locomotionForward,  current.locomotionForward);
    out.locomotionLateral = blend(lastBodyCommand_.locomotionLateral,  current.locomotionLateral);
    out.balanceAssist     = blend(lastBodyCommand_.balanceAssist,     current.balanceAssist);

    lastBodyCommand_ = out;
    return out;
}

void DeepImmersionPlatform::setRuntimePlatformProfile(RuntimePlatformProfile profile) { platformProfile_ = profile; }
RuntimePlatformProfile DeepImmersionPlatform::runtimePlatformProfile() const { return platformProfile_; }
NeuroPipelineDiagnostics DeepImmersionPlatform::lastDiagnostics() const { return lastDiagnostics_; }
void DeepImmersionPlatform::setConsentPolicy(const NeuroConsentPolicy& policy) { consentManager_.setPolicy(policy); }
void DeepImmersionPlatform::setInducedVector(const std::vector<float>& inducedVector) { inducedVector_ = inducedVector; }

} // namespace dnxr
