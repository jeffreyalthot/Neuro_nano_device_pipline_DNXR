#include "VirtualWorldState.h"
#include "../utils/Logger.h"

#include <algorithm>
#include <cmath>

namespace dnxr {

VirtualWorldState::VirtualWorldState()
    : sceneName_("BootChamber"),
      intensity_(0.5f),
      targetIntensity_(0.5f),
      transitionSpeed_(0.08f),
      dirtyFlag_(false),
      lastUpdate_(std::chrono::steady_clock::now()) {}

void VirtualWorldState::setScene(const std::string& newScene) {
    if (newScene.empty())
        return;

    std::lock_guard<std::mutex> lock(mtx_);
    if (sceneName_ != newScene) {
        sceneName_ = newScene;
        dirtyFlag_ = true;
        notifyObservers();
    }
}

void VirtualWorldState::setIntensity(float value) {
    value = clamp01(value);
    if (std::fabs(value - targetIntensity_.load()) < 1e-5f)
        return;

    targetIntensity_ = value;
    dirtyFlag_ = true;
}

void VirtualWorldState::update(double deltaTime) {
    std::lock_guard<std::mutex> lock(mtx_);

    // Transition lissée dans le temps
    float current = intensity_.load();
    float target = targetIntensity_.load();

    float step = std::max(0.0f, std::min(1.0f, static_cast<float>(deltaTime) * transitionSpeed_ * 60.0f));
    float blended = smoothBlend(current, target, step);

    intensity_ = blended;

    if (std::fabs(blended - target) < 0.001f) {
        if (dirtyFlag_) notifyObservers();
        dirtyFlag_ = false;
    }
}

void VirtualWorldState::integrateCognitiveInput(float emotionalLevel, float focusLevel) {
    // Influence des signaux cognitifs sur l’intensité :
    float modifier = (emotionalLevel * 0.6f) + (focusLevel * 0.4f);
    modifier = clamp01(modifier);

    setIntensity(modifier);

    if (modifier > 0.8f) {
        setScene("HighResonanceHall");
    } else if (modifier < 0.3f) {
        setScene("CalmSanctuary");
    } else {
        setScene("ExplorationCorridor");
    }
}

std::string VirtualWorldState::getSceneName() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return sceneName_;
}

float VirtualWorldState::getIntensity() const {
    return intensity_.load();
}

bool VirtualWorldState::isDirty() const {
    return dirtyFlag_.load();
}

void VirtualWorldState::setOnStateChanged(
    const std::function<void(const std::string&, float)>& cb) {
    std::lock_guard<std::mutex> lock(mtx_);
    onStateChanged_ = cb;
}

void VirtualWorldState::notifyObservers() const {
    if (onStateChanged_)
        onStateChanged_(sceneName_, intensity_.load());

    Logger::info("[VirtualWorldState] Scene=" + sceneName_ +
                 " | intensity=" + std::to_string(intensity_.load()));
}

float VirtualWorldState::clamp01(float v) {
    return std::max(0.0f, std::min(1.0f, v));
}

float VirtualWorldState::smoothBlend(float a, float b, float t) {
    return a + (b - a) * t;
}

} // namespace dnxr
