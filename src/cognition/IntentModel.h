#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <functional>

namespace dnxr {

struct FeatureStats {
    float mean = 0.0f;
    float min = 0.0f;
    float max = 0.0f;
    float stdev = 0.0f;
};

struct IntentResult {
    std::string intent;
    float score = 0.0f;
    float confidence = 0.0f;
};

/// Modèle d’intention cognitive :
/// établit une intention actuelle à partir d’un jeu de features neuronales/immersives.
class IntentModel {
public:
    IntentModel();

    /// Analyse un vecteur de caractéristiques et déduit une intention haute‑niveau.
    IntentResult classifyIntent(const std::vector<float>& features) const;

    /// Ajuste dynamiquement la sensibilité contextuelle du modèle.
    void setContextAdaptation(float volatilityThr, float immersionThr);

    /// Active lissage adaptatif des flux entrants.
    void enableSmoothing(bool enabled);

    /// Définit un callback appelé à chaque nouvelle intention détectée.
    void setOnIntentUpdated(const std::function<void(const IntentResult&)>& cb);

private:
    FeatureStats computeStats(const std::vector<float>& f) const;
    float computeImmersiveScore(const FeatureStats& s) const;
    float computeCalmScore(const FeatureStats& s) const;
    float computeFocusScore(const FeatureStats& s) const;

    IntentResult selectBestIntent(float immersive, float calm, float focus) const;
    float computeConfidence(const FeatureStats& stats, const std::string& intent) const;

    void emitIntent(const IntentResult& res) const;

private:
    float volatilityThreshold_;
    float immersionThreshold_;
    float calmMeanThreshold_;
    float calmMinThreshold_;
    bool smoothingEnabled_;

    std::function<void(const IntentResult&)> onIntentUpdated_;
    mutable std::mutex mtx_;
};

} // namespace dnxr
