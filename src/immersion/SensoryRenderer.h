#pragma once
// =============================================================================
// DOSSIER : src/immersion/
// FICHIER : SensoryRenderer.h
//
// OBJECTIF : Convertir le SenseFrame (5 sens + proprioception) produit par
//            le neural chip en une frame sensorielle multi-canaux complète
//            pour la stimulation XR directe via le chip.
//
// AMÉLIORATIONS NEURAL CHIP ONLY :
//   [A] renderFromNeuralChip() — méthode principale : reçoit SenseFrame +
//       NeuralSceneDescriptor + CognitiveState pour produire une frame
//       sensorielle sur 11 canaux (visuel×3, audio×2, haptique×2,
//       olfactif×1, gustatif×1, proprioceptif×1, vestibulaire×1).
//   [B] NeuralSensoryFrame — structure de sortie enrichie portant les
//       11 canaux nommés avec leur signification physique.
//   [C] Suppression des 3 canaux hardcodés (×1.0/×0.85/×0.65) :
//       chaque canal est maintenant piloté par une dimension du SenseFrame.
//   [D] applyNeuralGamma() — correction gamma adaptative par canal selon
//       l'état cognitif (pas de gamma fixe global).
//   [E] Compatibilité ascendante : renderSensoryFrame() conservé mais
//       délègue à renderFromNeuralChip() pour une transition propre.
// =============================================================================

#include "VirtualWorldState.h"
#include <vector>
#include <mutex>
#include <functional>

namespace dnxr {

struct CognitiveState;

// ─────────────────────────────────────────────────────────────────────────────
// [B] NeuralSensoryFrame — frame sensorielle complète sur 11 canaux
//
// Chaque canal représente une stimulation physique distincte envoyée
// au neural chip pour induction directe de la perception.
// Tous les canaux sont en [0, 1].
// ─────────────────────────────────────────────────────────────────────────────
struct NeuralSensoryFrame {
    // Canaux visuels (3)
    float visualPrimary     = 0.0f;  // luminance principale de la scène
    float visualDepth       = 0.0f;  // signal de profondeur stéréoscopique
    float visualColor       = 0.0f;  // saturation et teinte émotionnelle

    // Canaux auditifs (2)
    float audioSpatial      = 0.0f;  // immersion sonore 3D positionnelle
    float audioTonal        = 0.0f;  // richesse tonale et résonance

    // Canaux haptiques (2)
    float hapticPressure    = 0.0f;  // pression et résistance tactile
    float hapticVibration   = 0.0f;  // vibrations et textures

    // Canaux chimiosensoriels (2)
    float olfactory         = 0.0f;  // stimulation olfactive (odeur)
    float gustatory         = 0.0f;  // stimulation gustative (goût)

    // Canaux proprioceptifs / vestibulaires (2)
    float proprioceptive    = 0.0f;  // conscience de la position corporelle
    float vestibular        = 0.0f;  // équilibre et orientation spatiale

    // Conversion en vecteur float pour compatibilité avec le pipeline existant
    std::vector<float> toVector() const {
        return { visualPrimary, visualDepth, visualColor,
                 audioSpatial, audioTonal,
                 hapticPressure, hapticVibration,
                 olfactory, gustatory,
                 proprioceptive, vestibular };
    }
};

class SensoryRenderer {
public:
    SensoryRenderer();

    // ─────────────────────────────────────────────────────────────────────
    // [A] renderFromNeuralChip — MÉTHODE PRINCIPALE neural chip only
    //
    // Reçoit le SenseFrame (5 sens calculés depuis CognitiveState),
    // le NeuralSceneDescriptor (paramètres 3D) et le CognitiveState complet
    // pour produire une NeuralSensoryFrame sur 11 canaux.
    // Chaque canal est piloté par une ou plusieurs dimensions cognitives.
    // ─────────────────────────────────────────────────────────────────────
    NeuralSensoryFrame renderFromNeuralChip(
        const SenseFrame&             senses,
        const NeuralSceneDescriptor&  scene,
        const CognitiveState&         cognitive) const;

    // Compatibilité ascendante (délègue à renderFromNeuralChip)
    std::vector<float> renderSensoryFrame(const VirtualWorldState& state) const;

    // Réglages globaux du renderer
    void setChannelGain(float gain);
    float getChannelGain() const;
    void setGamma(float gamma);
    float getGamma() const;
    void enableAdaptiveBalance(bool enabled);
    bool isAdaptiveBalanceEnabled() const;
    void applyCognitiveModulation(float stressIndex, float focusIndex);
    void setOnFrameRendered(const std::function<void(const std::vector<float>&)>& cb);

private:
    // [D] Gamma adaptatif par canal selon l'état cognitif
    float applyNeuralGamma(float value, float cognitiveGamma) const;
    float applyChannelGain(float v) const;
    void  balanceNeuralChannels(NeuralSensoryFrame& frame) const;
    float computeCognitiveWeight(float stressIndex, float focusIndex) const;
    void  dispatchFrame(const std::vector<float>& frame) const;

private:
    float channelGain_;
    float gamma_;
    bool  adaptiveBalance_;
    float smoothingFactor_;
    float cognitiveWeight_;

    std::function<void(const std::vector<float>&)> onFrameRendered_;
    mutable std::mutex mtx_;
};

} // namespace dnxr
