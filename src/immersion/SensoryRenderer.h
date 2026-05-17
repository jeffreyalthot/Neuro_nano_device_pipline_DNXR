#pragma once

#include "VirtualWorldState.h"
#include <vector>
#include <mutex>
#include <functional>

namespace dnxr {

/// Gère la conversion d'un état de monde virtuel en une frame sensorielle XR multi-canaux.
/// Canaux interprétables comme intensités de stimulation visuelle, audio ou haptique.
class SensoryRenderer {
public:
    SensoryRenderer();

    /// Produit une frame sensorielle multi‑canaux à partir de l'état du monde.
    std::vector<float> renderSensoryFrame(const VirtualWorldState& state) const;

    /// Ajustement du gain global (énergie de sortie des canaux).
    void setChannelGain(float gain);
    float getChannelGain() const;

    /// Ajustement de la correction gamma (courbe perceptuelle).
    void setGamma(float gamma);
    float getGamma() const;

    /// (Dé)activation de la balance adaptative inter‑canaux.
    void enableAdaptiveBalance(bool enabled);
    bool isAdaptiveBalanceEnabled() const;

    /// Intègre une modulation supplémentaire issue d’un flux cognitif (stress, focus…)
    void applyCognitiveModulation(float stressIndex, float focusIndex);

    /// Enregistre un callback pour diffusion de la frame sensorielle finale.
    void setOnFrameRendered(const std::function<void(const std::vector<float>&)>& cb);

private:
    float applyGamma(float v) const;
    float applyChannelGain(float v) const;
    void balanceChannels(float channels[3]) const;
    float computeCognitiveWeight(float stressIndex, float focusIndex) const;
    void dispatchFrame(const std::vector<float>& frame) const;

private:
    float channelGain_;
    float gamma_;
    bool adaptiveBalance_;
    float smoothingFactor_;
    float cognitiveWeight_;  // facteur venant de la cognition

    const int channelCount_ = 3;
    std::function<void(const std::vector<float>&)> onFrameRendered_;

    mutable std::mutex mtx_;
};

} // namespace dnxr
