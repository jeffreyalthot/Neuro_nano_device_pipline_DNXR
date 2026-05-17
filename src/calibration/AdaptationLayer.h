#pragma once

#include <vector>
#include <mutex>
#include <atomic>
#include <chrono>
#include <functional>

namespace dnxr {

/// Interface d’adaptation dynamique : 
/// prépare les signaux entrants pour un pipeline immersif connecté.
class AdaptationLayer {
public:
    AdaptationLayer();

    /// Traite un vecteur de signaux sensoriels (extrait depuis XR, EEG ou API neuronale)
    /// et applique le profil utilisateur, la calibration temps réel et les gardes physiologiques.
    std::vector<float> processInputStream(
        const std::vector<float>& input,
        float userGain,
        double timestamp);

    /// Réglage global du gain de base.
    void setBaseGain(float gain);

    float getBaseGain() const;

    /// Plage de sortie physique finale (pour éviter les pics dangereux).
    void setClampRange(float minVal, float maxVal);

    /// Active/désactive les corrections non linéaires perceptuelles.
    void enableNonlinearMode(bool enabled);

    /// Active/désactive la normalisation statistique dynamique.
    void enableNormalization(bool enabled);

    /// Définit un callback optionnel pour émission des signaux calibrés (par ex. vers XRRenderer).
    void setOnAdaptedOutput(const std::function<void(const std::vector<float>&, double)>& cb);

private:
    // --- Fonctions internes ---

    float computeEffectiveGain(float userGain) const;
    float nonlinearTransform(float x) const;
    void normalize(std::vector<float>& data) const;
    void clampOutput(std::vector<float>& data) const;

    void emitAdapted(const std::vector<float>& data, double timestamp) const;

private:
    float baseGain_;
    float clampMin_;
    float clampMax_;
    bool nonlinearMode_;
    bool normalizationEnabled_;

    std::function<void(const std::vector<float>&, double)> onAdaptedOutput_;
    mutable std::mutex mtx_;
};

} // namespace dnxr
