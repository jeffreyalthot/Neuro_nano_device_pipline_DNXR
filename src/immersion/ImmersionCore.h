#pragma once
// =============================================================================
// DOSSIER : src/immersion/
// FICHIER : ImmersionCore.h
//
// AMÉLIORATIONS NEURAL CHIP ONLY :
//   [A] buildImmersiveFrameFromChip() — méthode principale qui reçoit le
//       triplet (CognitiveState, XRFullBodyCommand, XRAdaptationFrame,
//       NeuroPipelineDiagnostics) de DeepImmersionPlatform et orchestre
//       le pipeline complet pour produire une NeuralSensoryFrame 11 canaux.
//   [B] applyNeurofeedbackCue() — ferme la boucle neurofeedback en
//       réinjectant le cue cognitif dans le renderer et le monde 3D.
//   [C] getLastNeuralFrame() — accès à la dernière NeuralSensoryFrame pour
//       les modules downstream (SpatialAudioEngine, MultiSenseOrchestrator).
//   [D] lastNeuralFrame_ — stockage de la dernière frame complète.
// =============================================================================

#include "SensoryRenderer.h"
#include "VirtualWorldState.h"

#include <vector>
#include <cstdint>
#include <mutex>
#include <functional>

namespace dnxr {

struct CognitiveState;
struct XRFullBodyCommand;
struct XRAdaptationFrame;
struct NeuroPipelineDiagnostics;
struct NeuralSensoryFrame;

class ImmersionCore {
public:
    ImmersionCore();

    // ─────────────────────────────────────────────────────────────────────
    // [A] MÉTHODE PRINCIPALE neural chip only
    //
    // Orchestre le pipeline complet depuis les outputs de DeepImmersionPlatform.
    // Appelée par RuntimeController à chaque cycle (~90 Hz).
    // ─────────────────────────────────────────────────────────────────────
    NeuralSensoryFrame buildImmersiveFrameFromChip(
        const CognitiveState&           cognitive,
        const XRFullBodyCommand&        bodyCmd,
        const XRAdaptationFrame&        adaptation,
        const NeuroPipelineDiagnostics& diag);

    // Compatibilité ascendante (délègue au renderer via VirtualWorldState)
    std::vector<float> buildImmersiveFrame(const std::vector<float>& calibratedSignal);

    // [B] Fermeture de la boucle neurofeedback
    void applyNeurofeedbackCue(const std::string& cue, const CognitiveState& cognitive);

    // [C] Accès à la dernière frame neurale produite
    NeuralSensoryFrame getLastNeuralFrame() const;

    // Modulation externe (conservée)
    void applyCognitiveContext(float emotionalLevel, float focusLevel);
    void initializeBaseGameplay();
    void applyEmbodiedInput(const VirtualWorldState::EmbodiedInput& input,
                            double deltaTime = 0.016);
    std::string getAvatarPoseSummary() const;

    // Réglages dynamiques
    void setTransitionSpeed(float speed);
    float getTransitionSpeed() const;
    void enableAdaptiveMode(bool enabled);
    bool isAdaptiveModeEnabled() const;
    float getLastIntensity() const;
    uint64_t getFrameCount() const;
    void setOnFrameBuilt(const std::function<void(const std::vector<float>&)>& cb);

private:
    void postProcessFrame(std::vector<float>& frame) const;

private:
    SensoryRenderer    renderer_;
    VirtualWorldState  world_;

    float    transitionSpeed_;
    bool     adaptiveMode_;
    float    lastIntensity_;
    uint64_t frameCounter_;
    float    emotionalMod_;
    float    focusMod_;

    // [D] Dernière NeuralSensoryFrame produite
    NeuralSensoryFrame lastNeuralFrame_;

    mutable std::mutex mtx_;
    std::function<void(const std::vector<float>&)> onFrameBuilt_;
};

} // namespace dnxr
