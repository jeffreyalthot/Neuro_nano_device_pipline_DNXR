#pragma once

#include "SensoryRenderer.h"
#include "VirtualWorldState.h"

#include <vector>
#include <cstdint>
#include <mutex>
#include <functional>

namespace dnxr {

/// Noyau d’intégration neurale‑sensorielle :
/// transforme les signaux calibrés en stimuli immersifs XR cohérents.
class ImmersionCore {
public:
    ImmersionCore();

    /// Génère une frame sensorielle à partir d’un flux calibré.
    std::vector<float> buildImmersiveFrame(const std::vector<float>& calibratedSignal);

    /// Modulation externe (venant de flux cognitifs parallèles).
    void applyCognitiveContext(float emotionalLevel, float focusLevel);
    void initializeBaseGameplay() ;
    void applyEmbodiedInput(const VirtualWorldState::EmbodiedInput& input, double deltaTime = 0.016);
    std::string getAvatarPoseSummary() const;

    /// Réglages dynamiques.
    void setTransitionSpeed(float speed);
    float getTransitionSpeed() const;

    void enableAdaptiveMode(bool enabled);
    bool isAdaptiveModeEnabled() const;

    float getLastIntensity() const;
    uint64_t getFrameCount() const;

    /// Déclare un callback déclenché à chaque frame immersive produite.
    void setOnFrameBuilt(const std::function<void(const std::vector<float>&)>& cb);

private:
    void postProcessFrame(std::vector<float>& frame) const;

private:
    SensoryRenderer renderer_;
    VirtualWorldState world_;

    float transitionSpeed_;
    bool adaptiveMode_;

    float lastIntensity_;
    uint64_t frameCounter_;

    float emotionalMod_;
    float focusMod_;

    mutable std::mutex mtx_;
    std::function<void(const std::vector<float>&)> onFrameBuilt_;
};

} // namespace dnxr
