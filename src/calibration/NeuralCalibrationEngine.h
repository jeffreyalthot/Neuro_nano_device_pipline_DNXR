#pragma once

#include "AdaptationLayer.h"
#include <vector>
#include <mutex>
#include <functional>

namespace dnxr {

/// Moteur de calibration immersive connecté :
/// prépare les signaux neuronaux/ sensoriels pour un rendu XR profond.
class NeuralCalibrationEngine {
public:
    NeuralCalibrationEngine();

    /// Effectue une calibration complète des données brutes entrantes.
    /// Les données peuvent provenir : d’interfaces XR, EEG, ou NeuralBridge.
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

    /// Calcule le score d’immersion à partir d’un vecteur calibré.
    float computeImmersionScore(const std::vector<float>& calibrated) const;

    /// Définit un callback pour transmettre les signaux calibrés en sortie.
    void setOnCalibratedOutput(const std::function<void(const std::vector<float>&, double)>& cb);

private:
    void applyImmersionAmplification(std::vector<float>& signal) const;
    void reduceNeuralNoise(std::vector<float>& signal) const;
    void stabilizeSignal(std::vector<float>& signal) const;
    void applyEmotionalCompensation(std::vector<float>& signal) const;

    void emitCalibrated(const std::vector<float>& signal, double timestamp) const;

private:
    AdaptationLayer adaptation_;

    float immersionIntensity_;
    bool adaptiveNoiseReduction_;
    float emotionalFeedbackGain_;
    float stabilizationFactor_;

    std::function<void(const std::vector<float>&, double)> onCalibratedOutput_;
    mutable std::mutex mtx_;
};

} // namespace dnxr
