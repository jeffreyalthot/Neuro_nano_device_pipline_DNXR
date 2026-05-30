// =============================================================================
// DOSSIER : src/calibration/
// FICHIER : NeuralCalibrationEngine.h
//
// AMÉLIORATION APPLIQUÉE :
//   [5] Ajout de resetStabilization() (API publique) et de prevStabilization_
//       (membre mutable persistant) pour corriger le biais de début de frame
//       causé par l'initialisation locale à 0.0f dans stabilizeSignal().
// =============================================================================

#pragma once

#include "AdaptationLayer.h"
#include <vector>
#include <mutex>
#include <functional>

namespace dnxr {

/// Moteur de calibration immersive connecté :
/// prépare les signaux neuronaux / sensoriels pour un rendu XR profond.
class NeuralCalibrationEngine {
public:
    NeuralCalibrationEngine();

    /// Effectue une calibration complète des données brutes entrantes.
    /// Les données peuvent provenir : d'interfaces XR, EEG, ou NeuralBridge.
    std::vector<float> calibrateForImmersion(
        const std::vector<float>& features,
        float userSensitivity,
        double timestamp);

    /// --- Réglages principaux du pipeline ---
    void setImmersionIntensity(float intensity);
    float getImmersionIntensity() const;

    void enableAdaptiveNoiseReduction(bool enabled);
    bool isAdaptiveNoiseReductionEnabled() const;

    void setEmotionalFeedbackGain(float gain);
    float getEmotionalFeedbackGain() const;

    void setStabilizationFactor(float factor);
    float getStabilizationFactor() const;

    /// [AMÉLIORATION 5] Remet à zéro l'état interne du filtre IIR.
    /// À appeler uniquement sur recalibration explicite utilisateur,
    /// jamais automatiquement en début de frame.
    void resetStabilization();

    /// Calcule le score d'immersion à partir d'un vecteur calibré.
    float computeImmersionScore(const std::vector<float>& calibrated) const;

    /// Définit un callback pour transmettre les signaux calibrés en sortie.
    void setOnCalibratedOutput(const std::function<void(const std::vector<float>&, double)>& cb);

private:
    void applyImmersionAmplification(std::vector<float>& signal) const;
    void reduceNeuralNoise(std::vector<float>& signal) const;

    /// [AMÉLIORATION 5] stabilizeSignal utilise prevStabilization_ persistant
    /// au lieu d'une variable locale initialisée à 0.0f à chaque appel.
    void stabilizeSignal(std::vector<float>& signal) const;

    void applyEmotionalCompensation(std::vector<float>& signal) const;
    void emitCalibrated(const std::vector<float>& signal, double timestamp) const;

private:
    AdaptationLayer adaptation_;

    float immersionIntensity_;
    bool  adaptiveNoiseReduction_;
    float emotionalFeedbackGain_;
    float stabilizationFactor_;

    /// [AMÉLIORATION 5] État persistant du filtre IIR entre les frames.
    /// mutable car modifié dans stabilizeSignal() qui est const.
    mutable float prevStabilization_;

    std::function<void(const std::vector<float>&, double)> onCalibratedOutput_;
    mutable std::mutex mtx_;
};

} // namespace dnxr
