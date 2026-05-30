// =============================================================================
// DOSSIER : src/immersion/
// FICHIER : VirtualWorldState.cpp
//
// OBJECTIF : Piloter TOUT l'environnement 3D et les 5 sens depuis le
//            neural chip uniquement, sans aucune entrée physique externe.
//
// AMÉLIORATIONS NEURAL CHIP ONLY :
//   [A] applyNeuralChipInput() — point d'entrée unique. Reçoit CognitiveState
//       + XRFullBodyCommand + XRAdaptationFrame pour :
//       1. Déplacer l'avatar dans l'espace 3D (position, orientation)
//       2. Sélectionner la scène/biome selon l'état cognitif
//       3. Calculer les 5 sens (SenseFrame)
//       4. Produire le NeuralSceneDescriptor pour le rendu
//   [B] computeNeuralSenses() — mappe CognitiveState → SenseFrame complète.
//       Chaque sens est une combinaison linéaire pondérée de l'état neural.
//   [C] buildNeuralSceneDescriptor() — génère les paramètres de scène 3D
//       depuis CognitiveState + XRAdaptationFrame.
//   [D] updateAvatarFromBodyCommand() — pilotage de la locomotion 3D depuis
//       XRFullBodyCommand (issu de runFullBodyControlTick()).
//   [E] selectBiome() — sélection dynamique du biome selon l'état cognitif.
//   [F] selectLightingMode() — mode d'éclairage neural adaptatif.
// =============================================================================

#include "VirtualWorldState.h"
#include "../neurotech/DeepImmersionPlatform.h"
#include "../utils/Logger.h"

#include <algorithm>
#include <cmath>
#include <sstream>
#include <iomanip>

namespace dnxr {

namespace {

// Vitesse de déplacement de base dans le monde virtuel (unités/s)
// Modulée par motorIntent du chip pour immersion proportionnelle
constexpr float kBaseMovementSpeed  = 2.8f;
constexpr float kBaseRotationSpeed  = 90.0f; // degrés/s
constexpr float kPitchClampDeg      = 85.0f;
constexpr float kTransitionSpeed    = 0.10f;

float clamp01(float v)            { return std::max(0.0f, std::min(1.0f, v)); }
float smoothBlend(float a, float b, float t) { return a + (b - a) * t; }

} // namespace

VirtualWorldState::VirtualWorldState()
    : sceneName_("NeuralBootChamber"),
      intensity_(0.5f),
      targetIntensity_(0.5f),
      transitionSpeed_(kTransitionSpeed),
      dirtyFlag_(false),
      lastUpdate_(std::chrono::steady_clock::now()),
      posX_(0.0f), posY_(0.0f), posZ_(0.0f),
      yawDeg_(0.0f), pitchDeg_(0.0f),
      interactionActive_(false),
      senseBlend_(0.75f) {
    lastSceneDescriptor_.biomeName    = "neural_boot_void";
    lastSceneDescriptor_.lightingMode = "neutral_init";
}

// =============================================================================
// [A] applyNeuralChipInput — POINT D'ENTRÉE PRINCIPAL neural chip only
//
// Reçoit le triplet (CognitiveState, XRFullBodyCommand, XRAdaptationFrame)
// produits EXCLUSIVEMENT par DeepImmersionPlatform et :
//   1. Met à jour la position/orientation de l'avatar en 3D
//   2. Sélectionne le biome et l'éclairage selon l'état neural
//   3. Calcule les 5 sens (SenseFrame)
//   4. Construit le NeuralSceneDescriptor pour le pipeline de rendu
//   5. Ajuste l'intensité globale de la scène
// =============================================================================
void VirtualWorldState::applyNeuralChipInput(
    const CognitiveState&     cognitive,
    const XRFullBodyCommand&  bodyCmd,
    const XRAdaptationFrame&  adaptation,
    double                    deltaTime)
{
    std::lock_guard<std::mutex> lock(mtx_);
    const float dt = static_cast<float>(std::max(0.0, std::min(deltaTime, 0.1)));

    // [D] Locomotion 3D pilotée par les commandes full body du chip
    updateAvatarFromBodyCommand(bodyCmd, dt);

    // [E][F] Biome et éclairage générés depuis l'état cognitif neural
    updateSceneFromCognitive(cognitive, adaptation);

    // [B] Calcul des 5 sens depuis le vecteur cognitif neural
    lastSenseFrame_ = computeNeuralSenses(cognitive);

    // [C] Descripteur de scène 3D généré depuis le chip
    lastSceneDescriptor_ = buildNeuralSceneDescriptor(cognitive, adaptation);

    // Intensité globale de scène = fusion immersionIndex + sceneIntensity de l'adaptation
    const float targetIntensity = clamp01(
        cognitive.immersionIndex * 0.55f +
        adaptation.sceneIntensity  * 0.30f +
        cognitive.attention        * 0.15f
    );
    targetIntensity_ = targetIntensity;
    dirtyFlag_ = true;
}

// =============================================================================
// [D] updateAvatarFromBodyCommand — locomotion 3D depuis XRFullBodyCommand
//
// XRFullBodyCommand est issu de runFullBodyControlTick() qui lit le vecteur
// neural brut. Chaque composante contrôle directement un degré de liberté
// de l'avatar dans le monde 3D.
// =============================================================================
void VirtualWorldState::updateAvatarFromBodyCommand(
    const XRFullBodyCommand& cmd, float dt)
{
    // Vitesse de mouvement proportionnelle au motorIntent (déjà dans cmd)
    const float speedScale = kBaseMovementSpeed;

    // Locomotion 3D :
    // locomotionForward → avance/recule selon l'axe de regard (Z local)
    // locomotionLateral → strafe gauche/droite (X local)
    const float yawRad = yawDeg_ * 3.14159265f / 180.0f;
    const float fwd    = std::clamp(cmd.locomotionForward, -1.0f, 1.0f);
    const float lat    = std::clamp(cmd.locomotionLateral, -1.0f, 1.0f);

    posZ_ += (std::cos(yawRad) * fwd + std::sin(yawRad) * lat) * speedScale * dt;
    posX_ += (std::sin(yawRad) * fwd - std::cos(yawRad) * lat) * speedScale * dt;

    // Rotation tête/corps → yaw (rotation horizontale) et pitch (vertical)
    yawDeg_   += std::clamp(cmd.headYaw   + cmd.torsoYaw * 0.4f, -1.0f, 1.0f)
                 * kBaseRotationSpeed * dt;
    pitchDeg_  = std::clamp(
        pitchDeg_ + std::clamp(cmd.headPitch, -1.0f, 1.0f) * kBaseRotationSpeed * dt,
        -kPitchClampDeg, kPitchClampDeg);

    // Interaction déclenchée si la main dominante atteint un grip fort
    interactionActive_ = (cmd.rightHandGrip > 0.65f || cmd.leftHandGrip > 0.65f);
}

// =============================================================================
// [E][F] updateSceneFromCognitive — sélection de scène 3D par l'état neural
//
// Le biome et l'éclairage sont générés depuis les dimensions cognitives du
// chip, pas depuis des entrées utilisateur externes.
// =============================================================================
void VirtualWorldState::updateSceneFromCognitive(
    const CognitiveState&    cognitive,
    const XRAdaptationFrame& adaptation)
{
    const std::string newBiome   = selectBiome(cognitive);
    const std::string newLighting= selectLightingMode(cognitive);

    // La scène visible change si le biome ou l'éclairage évolue significativement
    const std::string newScene = newBiome + "_" + newLighting;
    if (newScene != sceneName_) {
        sceneName_ = newScene;
        dirtyFlag_ = true;
        notifyObservers();
        Logger::info("[VirtualWorldState] Neural scene transition → " + newScene);
    }
}

// =============================================================================
// [B] computeNeuralSenses — mappe CognitiveState → SenseFrame (5 sens + 2)
//
// Chaque sens est une combinaison pondérée de dimensions cognitives neurales.
// Les poids reflètent la neurobiologie : l'attention pilote le visuel et
// l'auditif, le motorIntent pilote le haptique, le stress module l'olfactif.
// =============================================================================
SenseFrame VirtualWorldState::computeNeuralSenses(const CognitiveState& cognitive)
{
    SenseFrame s;

    // ── Vision : clarté, profondeur et luminosité depuis le chip ──────────
    // Attention élevée → image mentale nette et profonde
    // Fatigue élevée  → image floue, profondeur réduite
    // Stress modéré   → contraste accru (effet vigilance)
    s.visualClarity    = clamp01(
        cognitive.attention    * 0.55f +
        (1.0f - cognitive.fatigue)  * 0.30f +
        cognitive.emotionValence    * 0.15f
    );
    s.visualDepth      = clamp01(
        cognitive.attention    * 0.60f +
        (1.0f - cognitive.stress)   * 0.25f +
        (1.0f - cognitive.fatigue)  * 0.15f
    );
    s.visualBrightness = clamp01(
        cognitive.immersionIndex * 0.50f +
        cognitive.emotionValence  * 0.30f +
        (1.0f - cognitive.fatigue) * 0.20f
    );

    // ── Audition : immersion sonore et directionnalité depuis le chip ─────
    // motorIntent fort → sons directionnels précis (spatialisation active)
    // immersionIndex  → richesse du paysage sonore
    s.auditoryImmersion = clamp01(
        cognitive.immersionIndex * 0.55f +
        cognitive.emotionValence * 0.30f +
        (1.0f - cognitive.stress) * 0.15f
    );
    s.auditoryDirection = clamp01(
        cognitive.attention  * 0.50f +
        cognitive.motorIntent * 0.35f +
        (1.0f - cognitive.fatigue) * 0.15f
    );

    // ── Toucher / Haptique : densité et fréquence depuis le chip ─────────
    // motorIntent est le proxy le plus direct du mouvement intentionnel
    // Stress modéré → sensibilité tactile accrue (survie)
    s.hapticDensity   = clamp01(
        cognitive.motorIntent  * 0.65f +
        cognitive.immersionIndex * 0.25f +
        cognitive.stress         * 0.10f
    );
    s.hapticFrequency = clamp01(
        cognitive.motorIntent  * 0.70f +
        (1.0f - cognitive.fatigue) * 0.20f +
        cognitive.attention        * 0.10f
    );

    // ── Olfactif : intensité de la stimulation chimiosensorielle ─────────
    // Valence émotionnelle positive → odeurs plaisantes (récompense)
    // Stress élevé → odeurs d'alerte (adrénaline, forêt brûlée...)
    // Immersion profonde → olfactif riche
    s.olfactoryIntensity = clamp01(
        cognitive.emotionValence  * 0.45f +
        cognitive.immersionIndex  * 0.35f +
        (1.0f - cognitive.stress) * 0.20f
    );

    // ── Gustatif : stimulation de la perception du goût ──────────────────
    // Canal le plus subtil — activé principalement en immersion profonde
    // et valence émotionnelle élevée (plaisir).
    s.gustatoryIntensity = clamp01(
        cognitive.emotionValence  * 0.50f +
        cognitive.immersionIndex  * 0.40f +
        cognitive.attention        * 0.10f
    );

    // ── Proprioception : conscience du corps dans l'espace 3D ────────────
    // motorIntent + attention → conscience précise de la position corporelle
    s.proprioceptiveLoad  = clamp01(
        cognitive.motorIntent * 0.55f +
        cognitive.attention    * 0.30f +
        (1.0f - cognitive.fatigue) * 0.15f
    );

    // ── Vestibulaire : stabilité de l'équilibre neural ───────────────────
    // Inverse du stress + attention → système vestibulaire stable
    // Fatigue ou stress élevés → instabilité (vertiges neuraux)
    s.vestibularStability = clamp01(
        (1.0f - cognitive.stress)  * 0.50f +
        (1.0f - cognitive.fatigue) * 0.35f +
        cognitive.attention         * 0.15f
    );

    return s;
}

// =============================================================================
// [C] buildNeuralSceneDescriptor — paramètres de scène 3D depuis le chip
//
// Toutes les propriétés de rendu de l'environnement 3D sont déterminées
// par l'état cognitif neural. Aucun paramètre n'est fixe ou externe.
// =============================================================================
NeuralSceneDescriptor VirtualWorldState::buildNeuralSceneDescriptor(
    const CognitiveState&    cognitive,
    const XRAdaptationFrame& adaptation)
{
    NeuralSceneDescriptor desc;

    // Luminosité ambiante : immersion profonde → lumière intérieure intense
    desc.ambientLuminosity = clamp01(
        adaptation.visualClarity   * 0.45f +
        cognitive.immersionIndex   * 0.35f +
        (1.0f - cognitive.fatigue) * 0.20f
    );

    // Distance de rendu : attention soutenue → horizon plus lointain
    desc.renderDistance = clamp01(
        cognitive.attention        * 0.55f +
        cognitive.immersionIndex   * 0.30f +
        (1.0f - cognitive.stress)  * 0.15f
    );

    // Particules : motorIntent actif + immersion élevée → effets riches
    desc.particleDensity = clamp01(
        cognitive.motorIntent      * 0.40f +
        cognitive.emotionValence   * 0.35f +
        cognitive.immersionIndex   * 0.25f
    );

    // Brouillard atmosphérique : stress ou fatigue élevés → scène plus trouble
    desc.fogDensity = clamp01(
        cognitive.fatigue  * 0.45f +
        cognitive.stress   * 0.35f +
        (1.0f - cognitive.immersionIndex) * 0.20f
    );

    // Saturation colorimétrique : valence émotionnelle → couleurs vives ou ternes
    desc.colorSaturation = clamp01(
        cognitive.emotionValence   * 0.55f +
        cognitive.attention        * 0.30f +
        (1.0f - cognitive.fatigue) * 0.15f
    );

    // Cohérence temporelle : signal de qualité = rendu stable (anti-flicker)
    // Fatigue ou stress élevés → scintillement neural possible
    desc.temporalCoherence = clamp01(
        (1.0f - cognitive.fatigue) * 0.50f +
        (1.0f - cognitive.stress)  * 0.35f +
        cognitive.attention         * 0.15f
    );

    // [E] Biome généré depuis le vecteur cognitif
    desc.biomeName    = selectBiome(cognitive);

    // [F] Mode d'éclairage neural adaptatif
    desc.lightingMode = selectLightingMode(cognitive);

    return desc;
}

// =============================================================================
// [E] selectBiome — biome de la scène 3D généré depuis l'état neural
//
// Le biome est une projection de l'état cognitif dans l'espace sémantique
// des environnements virtuels. Pas de sélection manuelle ni de menu.
// =============================================================================
std::string VirtualWorldState::selectBiome(const CognitiveState& cognitive)
{
    // Immersion très profonde + valence positive → environnement transcendant
    if (cognitive.immersionIndex > 0.85f && cognitive.emotionValence > 0.70f)
        return "crystal_resonance_void";

    // Focus intense + faible stress → forêt de concentration
    if (cognitive.attention > 0.80f && cognitive.stress < 0.30f)
        return "deep_focus_forest";

    // Stress élevé + motorIntent → arène de combat neural
    if (cognitive.stress > 0.70f && cognitive.motorIntent > 0.55f)
        return "adrenaline_combat_zone";

    // Fatigue élevée → chambre de récupération douce
    if (cognitive.fatigue > 0.75f)
        return "neural_recovery_sanctuary";

    // Valence émotionnelle négative + faible immersion → couloirs de transition
    if (cognitive.emotionValence < 0.35f)
        return "liminal_transit_corridor";

    // État équilibré et motivé → monde ouvert explorable
    if (cognitive.immersionIndex > 0.55f)
        return "open_world_highlands";

    // État neutre de départ
    return "neural_boot_chamber";
}

// =============================================================================
// [F] selectLightingMode — éclairage 3D adaptatif depuis le chip
// =============================================================================
std::string VirtualWorldState::selectLightingMode(const CognitiveState& cognitive)
{
    if (cognitive.immersionIndex > 0.80f && cognitive.stress < 0.40f)
        return "deep_immersion_glow";

    if (cognitive.stress > 0.65f)
        return "tension_redshift";

    if (cognitive.fatigue > 0.70f)
        return "dim_recovery_ambient";

    if (cognitive.attention > 0.75f)
        return "focus_zenith_bright";

    return "neural_balanced_daylight";
}

// ─── API de base conservée ───────────────────────────────────────────────────

void VirtualWorldState::setScene(const std::string& newScene) {
    if (newScene.empty()) return;
    std::lock_guard<std::mutex> lock(mtx_);
    if (sceneName_ != newScene) {
        sceneName_ = newScene;
        dirtyFlag_ = true;
        notifyObservers();
    }
}

void VirtualWorldState::setIntensity(float value) {
    value = clamp01(value);
    if (std::fabs(value - targetIntensity_.load()) < 1e-5f) return;
    targetIntensity_ = value;
    dirtyFlag_ = true;
}

void VirtualWorldState::update(double deltaTime) {
    std::lock_guard<std::mutex> lock(mtx_);
    float current = intensity_.load();
    float target  = targetIntensity_.load();
    float step    = std::max(0.0f, std::min(1.0f,
                    static_cast<float>(deltaTime) * transitionSpeed_ * 60.0f));
    intensity_    = smoothBlend(current, target, step);
    if (std::fabs(intensity_.load() - target) < 0.001f) {
        if (dirtyFlag_) notifyObservers();
        dirtyFlag_ = false;
    }
}

void VirtualWorldState::integrateCognitiveInput(float emotionalLevel, float focusLevel) {
    float modifier = clamp01(emotionalLevel * 0.6f + focusLevel * 0.4f);
    setIntensity(modifier);
    if      (modifier > 0.80f) setScene("HighResonanceHall");
    else if (modifier < 0.30f) setScene("CalmSanctuary");
    else                        setScene("ExplorationCorridor");
}

void VirtualWorldState::applyEmbodiedInput(const EmbodiedInput& input, double deltaTime) {
    std::lock_guard<std::mutex> lock(mtx_);
    const float dt = static_cast<float>(std::max(0.0, deltaTime));
    posZ_ += std::clamp(input.moveForward, -1.0f, 1.0f) * kBaseMovementSpeed * dt;
    posX_ += std::clamp(input.moveRight,   -1.0f, 1.0f) * kBaseMovementSpeed * dt;
    yawDeg_   += std::clamp(input.lookYaw,   -180.0f, 180.0f) * dt;
    pitchDeg_  = std::clamp(pitchDeg_ + std::clamp(input.lookPitch, -180.0f, 180.0f) * dt,
                             -kPitchClampDeg, kPitchClampDeg);
    interactionActive_ = input.interact;
    const float senses = clamp01(
        (input.vision + input.audition + input.touch + input.smell + input.taste) / 5.0f);
    senseBlend_ = senses;
    setIntensity(std::clamp(0.2f + senses * 0.8f, 0.0f, 1.0f));
}

std::string VirtualWorldState::getAvatarPoseSummary() const {
    std::lock_guard<std::mutex> lock(mtx_);
    std::ostringstream oss;
    oss << std::fixed << std::setprecision(2)
        << "x=" << posX_ << " y=" << posY_ << " z=" << posZ_
        << " yaw=" << yawDeg_ << " pitch=" << pitchDeg_
        << " interact=" << (interactionActive_ ? "1" : "0");
    return oss.str();
}

float            VirtualWorldState::getFiveSenseBlend()    const { std::lock_guard<std::mutex> l(mtx_); return senseBlend_; }
std::string      VirtualWorldState::getSceneName()         const { std::lock_guard<std::mutex> l(mtx_); return sceneName_; }
float            VirtualWorldState::getIntensity()         const { return intensity_.load(); }
bool             VirtualWorldState::isDirty()              const { return dirtyFlag_.load(); }
SenseFrame       VirtualWorldState::getLastSenseFrame()    const { std::lock_guard<std::mutex> l(mtx_); return lastSenseFrame_; }
NeuralSceneDescriptor VirtualWorldState::getLastSceneDescriptor() const {
    std::lock_guard<std::mutex> l(mtx_); return lastSceneDescriptor_; }

void VirtualWorldState::setOnStateChanged(
    const std::function<void(const std::string&, float)>& cb) {
    std::lock_guard<std::mutex> lock(mtx_);
    onStateChanged_ = cb;
}

void VirtualWorldState::notifyObservers() const {
    if (onStateChanged_) onStateChanged_(sceneName_, intensity_.load());
    Logger::info("[VirtualWorldState] Scene=" + sceneName_ +
                 " | intensity=" + std::to_string(intensity_.load()));
}

float VirtualWorldState::clamp01(float v) { return std::max(0.0f, std::min(1.0f, v)); }
float VirtualWorldState::smoothBlend(float a, float b, float t) { return a + (b - a) * t; }

} // namespace dnxr
