// =============================================================================
// DOSSIER : src/immersion/
// FICHIER : ImmersionCore.cpp
//
// OBJECTIF : Noyau central de l'immersion profonde neural chip only.
//            Orchestre le pipeline complet : CognitiveState → SenseFrame
//            → NeuralSensoryFrame → VirtualWorldState 3D → stimulation chip.
//
// AMÉLIORATIONS NEURAL CHIP ONLY :
//   [A] buildImmersiveFrameFromChip() — nouvelle méthode principale.
//       Reçoit CognitiveState + XRFullBodyCommand + XRAdaptationFrame
//       directement depuis DeepImmersionPlatform et :
//       1. Applique les commandes corporelles au monde 3D (locomotion)
//       2. Calcule les 5 sens via computeNeuralSenses()
//       3. Produit la NeuralSensoryFrame (11 canaux) via SensoryRenderer
//       4. Ferme la boucle de neurofeedback en informant le renderer
//          du cue cognitif courant (neurofeedbackCue → ajustement rendu)
//   [B] buildImmersiveFrame() conservé pour compatibilité mais délègue
//       à buildImmersiveFrameFromChip() via un CognitiveState reconstruit.
//   [C] applyNeurofeedbackCue() — BOUCLE FERMÉE : le neurofeedbackCue de
//       DeepImmersionPlatform ajuste en temps réel le renderer et le monde.
//       C'est le maillon manquant de la v1.9.0 qui est ajouté ici.
//   [D] lastNeuralFrame_ — stocke la NeuralSensoryFrame complète pour
//       accès externe (RuntimeController, SpatialAudioEngine).
// =============================================================================

#include "ImmersionCore.h"
#include "GameDesignBridge.h"
#include "../neurotech/DeepImmersionPlatform.h"
#include "../utils/Logger.h"

#include <algorithm>
#include <cmath>
#include <numeric>

namespace dnxr {

namespace {
float clampIntensity(float v) { return std::clamp(v, 0.0f, 1.5f); }
float clamp01(float v)        { return std::clamp(v, 0.0f, 1.0f); }
float smoothStep(float current, float target, float factor) {
    return current + (target - current) * factor;
}

std::vector<float> composeRenderFrame(
    const NeuralSensoryFrame& sensory,
    const VirtualViewFrame& view,
    const VisualFieldFrame& visualField)
{
    std::vector<float> frame = sensory.toVector();
    const std::vector<float> viewChannels = view.toRenderVector();
    const std::vector<float> fieldChannels = visualField.toRenderVector();
    frame.reserve(frame.size() + viewChannels.size() + fieldChannels.size());
    frame.insert(frame.end(), viewChannels.begin(), viewChannels.end());
    frame.insert(frame.end(), fieldChannels.begin(), fieldChannels.end());
    return frame;
}
} // namespace

ImmersionCore::ImmersionCore()
    : transitionSpeed_(0.12f),
      adaptiveMode_(true),
      lastIntensity_(0.5f),
      frameCounter_(0),
      emotionalMod_(0.5f),
      focusMod_(0.5f) {}

// =============================================================================
// [A] buildImmersiveFrameFromChip — PIPELINE PRINCIPAL neural chip only
//
// Ce method est appelée par RuntimeController à chaque cycle (~90 Hz).
// Elle reçoit les 3 outputs de DeepImmersionPlatform :
//   • cognitive  → état mental complet (attention, stress, immersionIndex…)
//   • bodyCmd    → commandes corporelles 3D (locomotion, grip, orientation)
//   • adaptation → paramètres d'adaptation XR (clarté, shader, haptique…)
//   • diag       → diagnostics pipeline (neurofeedbackCue pour boucle fermée)
//
// Retourne la NeuralSensoryFrame sur 11 canaux pour stimulation directe.
// =============================================================================
NeuralSensoryFrame ImmersionCore::buildImmersiveFrameFromChip(
    const CognitiveState&         cognitive,
    const XRFullBodyCommand&      bodyCmd,
    const XRAdaptationFrame&      adaptation,
    const NeuroPipelineDiagnostics& diag)
{
    std::lock_guard<std::mutex> lock(mtx_);

    // ── Étape 1 : Piloter le monde 3D depuis le chip ─────────────────────
    // Position/orientation avatar + biome + éclairage mis à jour neuralement
    world_.applyNeuralChipInput(cognitive, bodyCmd, adaptation, 0.016);
    world_.update(0.016);

    // Conserver la luminosité de scène pour le log
    lastIntensity_ = world_.getIntensity();

    // ── Étape 2 : Récupérer le SenseFrame et le NeuralSceneDescriptor ────
    const SenseFrame             senses = world_.getLastSenseFrame();
    const NeuralSceneDescriptor  scene  = world_.getLastSceneDescriptor();
    const VirtualViewFrame       view   = world_.buildVirtualViewFrame();
    const VisualFieldFrame       visualField = world_.buildVisualFieldFrame();

    // ── Étape 3 : [C] Boucle de neurofeedback fermée ─────────────────────
    // Le cue du pipeline (ex: "breathe_and_reduce_fx", "micro_rest_window")
    // ajuste les paramètres du renderer en temps réel.
    applyNeurofeedbackCue(diag.neurofeedbackCue, cognitive);

    // ── Étape 4 : Rendu neural sur 11 canaux sensoriels ──────────────────
    const NeuralSensoryFrame neuralFrame =
        renderer_.renderFromNeuralChip(senses, scene, cognitive);

    // ── Étape 5 : Stocker et diffuser ─────────────────────────────────────
    lastNeuralFrame_ = neuralFrame;
    lastViewFrame_ = view;
    lastVisualFieldFrame_ = visualField;
    lastCompositeRenderFrame_ = composeRenderFrame(neuralFrame, view, visualField);
    ++frameCounter_;

    if (onFrameBuilt_) onFrameBuilt_(lastCompositeRenderFrame_);

    Logger::info("[ImmersionCore] NeuralFrame #" + std::to_string(frameCounter_) +
                 " | scene=" + world_.getSceneName() +
                 " | biome=" + scene.biomeName +
                 " | fov=" + std::to_string(view.fieldOfViewDeg) +
                 " | visualField=" + std::to_string(visualField.horizontalFovDeg) + "x" +
                 std::to_string(visualField.verticalFovDeg) +
                 " | compositeChannels=" + std::to_string(lastCompositeRenderFrame_.size()) +
                 " | intensity=" + std::to_string(lastIntensity_) +
                 " | cue=" + diag.neurofeedbackCue);

    return neuralFrame;
}

// =============================================================================
// [C] applyNeurofeedbackCue — BOUCLE DE FEEDBACK FERMÉE
//
// Le neurofeedbackCue calculé dans runInferenceTick() est réinjecté ici
// pour ajuster en temps réel le renderer et le monde virtuel.
//
// C'était LA lacune principale de la v1.9.0 : le cue était calculé mais
// jamais réutilisé. Cette méthode ferme la boucle neural → rendu → neural.
//
// Effets par cue :
//   "breathe_and_reduce_fx"   → baisser gain global + brouillard doux
//   "micro_rest_window"        → réduire gain + ralentir transitions
//   "anchor_attention_target"  → augmenter clarté visuelle + saturation
//   "maintain_deep_dive"       → gain plein + rendu hyperréel
//   "adaptive_balance"         → paramètres nominaux
// =============================================================================
void ImmersionCore::applyNeurofeedbackCue(
    const std::string&    cue,
    const CognitiveState& cognitive)
{
    if (cue == "breathe_and_reduce_fx") {
        // Stress élevé : réduire la charge sensorielle
        renderer_.setChannelGain(0.65f);
        renderer_.setGamma(2.2f);  // gamma plus dur = image plus sombre/calme
        transitionSpeed_ = 0.06f;  // transitions plus lentes = moins stressant

    } else if (cue == "micro_rest_window") {
        // Fatigue élevée : immersion réduite pour récupération cognitive
        renderer_.setChannelGain(0.55f);
        renderer_.setGamma(2.0f);
        transitionSpeed_ = 0.04f;

    } else if (cue == "anchor_attention_target") {
        // Attention faible : accentuer les stimuli pour ramener le focus
        renderer_.setChannelGain(1.10f);
        renderer_.setGamma(1.5f);  // gamma doux = image lumineuse et nette
        transitionSpeed_ = 0.18f;  // transitions vives

    } else if (cue == "maintain_deep_dive") {
        // Immersion profonde optimale : gain plein, rendu hyperréel
        renderer_.setChannelGain(clamp01(0.90f + cognitive.immersionIndex * 0.20f));
        renderer_.setGamma(1.2f);
        transitionSpeed_ = 0.14f;

    } else {
        // "adaptive_balance" ou inconnu : paramètres nominaux
        renderer_.setChannelGain(0.85f);
        renderer_.setGamma(1.8f);
        transitionSpeed_ = 0.12f;
    }

    // Propager la modulation cognitive au renderer
    renderer_.applyCognitiveModulation(cognitive.stress, cognitive.attention);
}

// =============================================================================
// [B] buildImmersiveFrame — conservé pour compatibilité ascendante
//
// Reconstruit un contexte minimal depuis le signal calibré et délègue
// au renderer via l'ancienne API du VirtualWorldState.
// =============================================================================
std::vector<float> ImmersionCore::buildImmersiveFrame(
    const std::vector<float>& calibratedSignal)
{
    std::lock_guard<std::mutex> lock(mtx_);

    if (!calibratedSignal.empty()) {
        const float modFactor = std::clamp(
            (emotionalMod_ * 0.6f + focusMod_ * 0.4f), 0.25f, 1.25f);
        const float rawTarget = clampIntensity(calibratedSignal[0] * modFactor);

        if (adaptiveMode_) {
            float blended = smoothStep(world_.getIntensity(), rawTarget, transitionSpeed_);
            world_.setIntensity(blended);
        } else {
            world_.setIntensity(rawTarget);
        }

        lastIntensity_ = world_.getIntensity();
        world_.integrateCognitiveInput(emotionalMod_, focusMod_);
    }

    world_.update();
    auto frame = renderer_.renderSensoryFrame(world_);
    postProcessFrame(frame);
    ++frameCounter_;

    if (onFrameBuilt_) onFrameBuilt_(frame);

    Logger::info("[ImmersionCore] Legacy frame #" + std::to_string(frameCounter_) +
                 " | intensity=" + std::to_string(lastIntensity_) +
                 " | scene=" + world_.getSceneName());

    return frame;
}

NeuralSensoryFrame ImmersionCore::getLastNeuralFrame() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return lastNeuralFrame_;
}

VirtualViewFrame ImmersionCore::getLastVirtualViewFrame() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return lastViewFrame_;
}

VisualFieldFrame ImmersionCore::getLastVisualFieldFrame() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return lastVisualFieldFrame_;
}

std::vector<float> ImmersionCore::getLastCompositeRenderFrame() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return lastCompositeRenderFrame_;
}

void ImmersionCore::applyCognitiveContext(float emotionalLevel, float focusLevel) {
    emotionalMod_ = std::clamp(emotionalLevel, 0.0f, 1.0f);
    focusMod_     = std::clamp(focusLevel,     0.0f, 1.0f);
    renderer_.applyCognitiveModulation(emotionalMod_, focusMod_);
}

void ImmersionCore::initializeBaseGameplay() {
    std::lock_guard<std::mutex> lock(mtx_);
    GameDesignBridge bridge;
    bridge.integrateBaseGameplaySystems(world_);
}

void ImmersionCore::applyEmbodiedInput(
    const VirtualWorldState::EmbodiedInput& input, double deltaTime) {
    std::lock_guard<std::mutex> lock(mtx_);
    world_.applyEmbodiedInput(input, deltaTime);
}

std::string ImmersionCore::getAvatarPoseSummary() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return world_.getAvatarPoseSummary();
}

void ImmersionCore::setTransitionSpeed(float speed) {
    transitionSpeed_ = std::clamp(speed, 0.01f, 1.0f); }
float ImmersionCore::getTransitionSpeed() const { return transitionSpeed_; }
void  ImmersionCore::enableAdaptiveMode(bool enabled) { adaptiveMode_ = enabled; }
bool  ImmersionCore::isAdaptiveModeEnabled() const    { return adaptiveMode_; }
float ImmersionCore::getLastIntensity() const          { return lastIntensity_; }
uint64_t ImmersionCore::getFrameCount() const         { return frameCounter_; }

void ImmersionCore::setOnFrameBuilt(
    const std::function<void(const std::vector<float>&)>& cb) {
    std::lock_guard<std::mutex> lock(mtx_);
    onFrameBuilt_ = cb;
}

void ImmersionCore::postProcessFrame(std::vector<float>& frame) const {
    if (frame.empty()) return;
    float avg = std::accumulate(frame.begin(), frame.end(), 0.0f)
              / static_cast<float>(frame.size());
    for (float& v : frame)
        v = (0.85f * v) + (0.15f * avg);
}

} // namespace dnxr
