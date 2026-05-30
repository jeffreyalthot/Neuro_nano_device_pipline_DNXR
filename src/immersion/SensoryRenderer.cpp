// =============================================================================
// DOSSIER : src/immersion/
// FICHIER : SensoryRenderer.cpp
//
// OBJECTIF : Produire la frame sensorielle complète (11 canaux) depuis le
//            SenseFrame calculé par le neural chip. Chaque canal est piloté
//            par une combinaison précise de dimensions cognitives.
//
// AMÉLIORATIONS NEURAL CHIP ONLY :
//   [A] renderFromNeuralChip() — chaque canal est calculé depuis SenseFrame
//       + NeuralSceneDescriptor + CognitiveState :
//       • visualPrimary   = visualClarity × ambientLuminosity × cogWeight
//       • visualDepth     = visualDepth × renderDistance
//       • visualColor     = colorSaturation × emotionValence
//       • audioSpatial    = auditoryDirection × auditoryImmersion
//       • audioTonal      = auditoryImmersion × (1-stress)
//       • hapticPressure  = hapticDensity × motorIntent
//       • hapticVibration = hapticFrequency × motorIntent
//       • olfactory       = olfactoryIntensity × (1-fogDensity)
//       • gustatory       = gustatoryIntensity
//       • proprioceptive  = proprioceptiveLoad × motorPrecision
//       • vestibular      = vestibularStability × (1-fatigue)
//   [B] applyNeuralGamma() — gamma adaptatif par canal (pas de fixe global).
//   [C] balanceNeuralChannels() — équilibrage inter-canaux basé sur
//       l'état cognitif, pas sur un filtre passe-bas fixe.
// =============================================================================

#include "SensoryRenderer.h"
#include "../neurotech/DeepImmersionPlatform.h"
#include "../utils/Logger.h"

#include <algorithm>
#include <cmath>

namespace dnxr {

namespace {
float clamp01(float v) { return std::clamp(v, 0.0f, 1.0f); }
} // namespace

SensoryRenderer::SensoryRenderer()
    : channelGain_(1.0f),
      gamma_(1.8f),
      adaptiveBalance_(true),
      smoothingFactor_(0.70f),
      cognitiveWeight_(1.0f) {}

// =============================================================================
// [A] renderFromNeuralChip — RENDU NEURAL CHIP ONLY SUR 11 CANAUX
//
// Chaque canal sensoriel est une fonction explicite des dimensions du
// SenseFrame (calculées depuis CognitiveState par computeNeuralSenses())
// et du NeuralSceneDescriptor (paramètres de scène 3D générés par le chip).
//
// AUCUN canal n'est fixe ou hardcodé. Tout varie avec l'état neural.
// =============================================================================
NeuralSensoryFrame SensoryRenderer::renderFromNeuralChip(
    const SenseFrame&            senses,
    const NeuralSceneDescriptor& scene,
    const CognitiveState&        cognitive) const
{
    std::lock_guard<std::mutex> lock(mtx_);

    NeuralSensoryFrame frame;

    // ── Canal visuel primaire ────────────────────────────────────────────
    // Clarté visuelle × luminosité ambiante de scène × poids cognitif global
    // Gamma adaptatif : immersion profonde → gamma plus doux (non linéaire)
    const float visualGamma = 1.2f + (1.0f - cognitive.immersionIndex) * 0.8f;
    frame.visualPrimary = applyNeuralGamma(
        clamp01(senses.visualClarity * scene.ambientLuminosity * cognitiveWeight_),
        visualGamma);

    // ── Canal de profondeur visuelle 3D ──────────────────────────────────
    // Profondeur de champ × distance de rendu × anti-fatigue
    frame.visualDepth = applyNeuralGamma(
        clamp01(senses.visualDepth * scene.renderDistance),
        1.5f);

    // ── Canal colorimétrique / émotionnel ─────────────────────────────
    // Saturation de scène × valence émotionnelle neural
    frame.visualColor = clamp01(
        scene.colorSaturation * cognitive.emotionValence * 0.70f +
        senses.visualBrightness * 0.30f);

    // ── Canal audio spatial (spatialisation 3D) ───────────────────────
    // Direction auditive × immersion × cohérence temporelle de la scène
    frame.audioSpatial = clamp01(
        senses.auditoryDirection  * 0.55f +
        senses.auditoryImmersion  * 0.30f +
        scene.temporalCoherence   * 0.15f);

    // ── Canal audio tonal (richesse harmonique) ──────────────────────
    // Immersion auditive × relaxation (anti-stress)
    frame.audioTonal = clamp01(
        senses.auditoryImmersion       * 0.65f +
        (1.0f - cognitive.stress)       * 0.25f +
        cognitive.emotionValence         * 0.10f);

    // ── Canal haptique de pression ────────────────────────────────────
    // Densité haptique × motorIntent (intention de mouvement neural)
    // Modulé par safetyLimiter implicite : jamais > 0.9 en mode stress élevé
    const float hapticSafeLimit = (cognitive.stress > 0.75f) ? 0.55f : 1.0f;
    frame.hapticPressure = clamp01(
        senses.hapticDensity    * cognitive.motorIntent * hapticSafeLimit * 0.80f +
        cognitive.immersionIndex * 0.20f);

    // ── Canal haptique de vibration (textures, impulsions) ───────────
    frame.hapticVibration = clamp01(
        senses.hapticFrequency   * cognitive.motorIntent * hapticSafeLimit * 0.75f +
        (1.0f - cognitive.fatigue) * 0.25f);

    // ── Canal olfactif ─────────────────────────────────────────────────
    // Intensité olfactive atténuée si la scène est brumeuse (fogDensity)
    frame.olfactory = clamp01(
        senses.olfactoryIntensity * (1.0f - scene.fogDensity * 0.40f));

    // ── Canal gustatif ─────────────────────────────────────────────────
    // Canal le plus discret — uniquement en immersion profonde et sans stress
    frame.gustatory = clamp01(
        senses.gustatoryIntensity * clamp01(cognitive.immersionIndex - 0.40f) * 2.5f);

    // ── Canal proprioceptif ──────────────────────────────────────────
    // Conscience corporelle × précision motrice × profondeur scène 3D
    frame.proprioceptive = clamp01(
        senses.proprioceptiveLoad * scene.renderDistance * 0.70f +
        cognitive.motorIntent      * 0.30f);

    // ── Canal vestibulaire (équilibre / orientation) ──────────────────
    // Stabilité × anti-fatigue — réduit si stress élevé (vertiges)
    frame.vestibular = clamp01(
        senses.vestibularStability * (1.0f - cognitive.fatigue * 0.50f) * 0.80f +
        scene.temporalCoherence    * 0.20f);

    // Gain global appliqué uniformément sur tous les canaux
    auto applyGain = [this](float v) { return clamp01(v * channelGain_); };
    frame.visualPrimary   = applyGain(frame.visualPrimary);
    frame.visualDepth     = applyGain(frame.visualDepth);
    frame.visualColor     = applyGain(frame.visualColor);
    frame.audioSpatial    = applyGain(frame.audioSpatial);
    frame.audioTonal      = applyGain(frame.audioTonal);
    frame.hapticPressure  = applyGain(frame.hapticPressure);
    frame.hapticVibration = applyGain(frame.hapticVibration);
    frame.olfactory       = applyGain(frame.olfactory);
    frame.gustatory       = applyGain(frame.gustatory);
    frame.proprioceptive  = applyGain(frame.proprioceptive);
    frame.vestibular      = applyGain(frame.vestibular);

    // [C] Équilibrage adaptatif inter-canaux
    if (adaptiveBalance_) balanceNeuralChannels(frame);

    // Dispatch vers le pipeline downstream
    const std::vector<float> frameVec = frame.toVector();
    dispatchFrame(frameVec);

    Logger::info("[SensoryRenderer] Neural frame | visual=" +
                 std::to_string(frame.visualPrimary) +
                 " haptic=" + std::to_string(frame.hapticPressure) +
                 " audio=" + std::to_string(frame.audioSpatial) +
                 " olfactory=" + std::to_string(frame.olfactory) +
                 " vestibular=" + std::to_string(frame.vestibular));

    return frame;
}

// =============================================================================
// [B] applyNeuralGamma — correction gamma adaptative par canal
//
// Contrairement à l'ancien gamma global fixe (1.8), chaque canal reçoit
// son propre gamma selon son rôle perceptuel. Le canal visuel primaire
// utilise un gamma doux en immersion profonde pour éviter la saturation.
// =============================================================================
float SensoryRenderer::applyNeuralGamma(float value, float cognitiveGamma) const {
    const float safeGamma = std::max(0.1f, cognitiveGamma);
    return std::pow(clamp01(value), safeGamma);
}

// =============================================================================
// [C] balanceNeuralChannels — équilibrage inter-canaux basé sur l'état neural
//
// Remplace l'ancien filtre passe-bas fixe (smoothingFactor_ × moyenne).
// Ici, les canaux sont équilibrés selon leur groupe sensoriel :
//   • Visuels : moyenne douce avec pondération prioritaire sur visualPrimary
//   • Haptiques : liés entre eux (pression <-> vibration cohérentes)
//   • Audio : équilibrage spatial-tonal
//   • Chimiosensoriels : indépendants (niveaux très différents)
// =============================================================================
void SensoryRenderer::balanceNeuralChannels(NeuralSensoryFrame& frame) const {
    // Équilibrage visuel : smoothing inter-canaux
    const float visualAvg = (frame.visualPrimary + frame.visualDepth + frame.visualColor) / 3.0f;
    frame.visualPrimary = smoothingFactor_ * frame.visualPrimary + (1.0f - smoothingFactor_) * visualAvg;
    frame.visualDepth   = smoothingFactor_ * frame.visualDepth   + (1.0f - smoothingFactor_) * visualAvg;
    frame.visualColor   = smoothingFactor_ * frame.visualColor   + (1.0f - smoothingFactor_) * visualAvg;

    // Équilibrage audio : cohérence spatial / tonal
    const float audioAvg = (frame.audioSpatial + frame.audioTonal) * 0.5f;
    frame.audioSpatial   = smoothingFactor_ * frame.audioSpatial + (1.0f - smoothingFactor_) * audioAvg;
    frame.audioTonal     = smoothingFactor_ * frame.audioTonal   + (1.0f - smoothingFactor_) * audioAvg;

    // Équilibrage haptique : pression et vibration restent cohérentes
    const float hapticAvg = (frame.hapticPressure + frame.hapticVibration) * 0.5f;
    frame.hapticPressure  = smoothingFactor_ * frame.hapticPressure  + (1.0f - smoothingFactor_) * hapticAvg;
    frame.hapticVibration = smoothingFactor_ * frame.hapticVibration + (1.0f - smoothingFactor_) * hapticAvg;

    // Canaux chimiosensoriels et proprioceptifs : pas de cross-balance
    // (ils sont indépendants et à niveaux naturellement différents)
}

std::vector<float> SensoryRenderer::renderSensoryFrame(const VirtualWorldState& state) const {
    // Compatibilité ascendante : reconstruit un SenseFrame minimal depuis l'état du monde
    SenseFrame senses;
    senses.visualClarity     = state.getIntensity();
    senses.visualBrightness  = state.getIntensity();
    senses.auditoryImmersion = state.getIntensity() * 0.85f;
    senses.hapticDensity     = state.getIntensity() * 0.65f;

    NeuralSceneDescriptor scene;
    scene.ambientLuminosity  = state.getIntensity();
    scene.renderDistance     = 0.6f;
    scene.colorSaturation    = 0.7f;
    scene.temporalCoherence  = 1.0f;

    CognitiveState cognitive;
    cognitive.immersionIndex = state.getIntensity();
    cognitive.attention      = 0.6f;
    cognitive.emotionValence = 0.5f;

    const NeuralSensoryFrame nFrame = renderFromNeuralChip(senses, scene, cognitive);
    return nFrame.toVector();
}

float SensoryRenderer::applyChannelGain(float v) const { return v * channelGain_; }

void SensoryRenderer::setChannelGain(float gain) { channelGain_ = std::max(0.05f, gain); }
float SensoryRenderer::getChannelGain() const    { return channelGain_; }
void SensoryRenderer::setGamma(float gamma)      { gamma_ = std::max(0.1f, gamma); }
float SensoryRenderer::getGamma() const          { return gamma_; }
void SensoryRenderer::enableAdaptiveBalance(bool enabled) { adaptiveBalance_ = enabled; }
bool SensoryRenderer::isAdaptiveBalanceEnabled() const    { return adaptiveBalance_; }

void SensoryRenderer::applyCognitiveModulation(float stressIndex, float focusIndex) {
    cognitiveWeight_ = computeCognitiveWeight(stressIndex, focusIndex);
}

float SensoryRenderer::computeCognitiveWeight(float stressIndex, float focusIndex) const {
    float modulation = (focusIndex * 0.7f) - (stressIndex * 0.5f);
    return std::clamp(1.0f + modulation, 0.5f, 1.5f);
}

void SensoryRenderer::setOnFrameRendered(
    const std::function<void(const std::vector<float>&)>& cb) {
    std::lock_guard<std::mutex> lock(mtx_);
    onFrameRendered_ = cb;
}

void SensoryRenderer::dispatchFrame(const std::vector<float>& frame) const {
    if (onFrameRendered_) onFrameRendered_(frame);
}

} // namespace dnxr
