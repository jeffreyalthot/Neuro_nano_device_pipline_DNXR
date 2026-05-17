#pragma once

#include "IntentModel.h"
#include <string>
#include <vector>
#include <mutex>
#include <functional>
#include <chrono>

namespace dnxr {

/// Analyse cognitive temps réel.
/// Déduit un état immersif ou cognitif à partir des vecteurs calibrés
/// issus du pipeline Neural ↔ XR.
class StateInferenceEngine {
public:
    StateInferenceEngine();

    /// Infère un état cognitif enrichi à partir des features actuelles.
    /// Accepte un timestamp pour cohérence temporelle inter‑modules.
    std::string inferState(
        const std::vector<float>& features,
        double timestamp);

    // --- Réglages dynamiques ---
    void setConfidenceThreshold(float threshold);
    float getConfidenceThreshold() const;

    void setSmoothingFactor(float factor);
    float getSmoothingFactor() const;

    void setContextWindowSize(size_t size);
    size_t getContextWindowSize() const;

    /// Callback optionnel pour relayer les états inférés
    /// (vers moteur XR, adaptateur NeuralBridge, ou logs).
    void setOnStateInferred(const std::function<void(const std::string&, double)>& cb);

private:
    std::vector<float> extractContext(const std::vector<float>& input) const;
    void smoothContext(std::vector<float>& data) const;
    std::string buildStateLabel(const IntentResult& result) const;
    void emitState(const std::string& state, double timestamp) const;

private:
    IntentModel intent_;

    float confidenceThreshold_;
    float smoothingFactor_;
    size_t contextWindowSize_;

    std::function<void(const std::string&, double)> onStateInferred_;
    mutable std::mutex mtx_;
};

} // namespace dnxr
