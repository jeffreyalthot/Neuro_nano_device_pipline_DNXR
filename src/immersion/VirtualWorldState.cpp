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
      lastUpdate_(std::chrono::steady_clock::now()),
      posX_(0.0f),
      posY_(0.0f),
      posZ_(0.0f),
      yawDeg_(0.0f),
      pitchDeg_(0.0f),
      interactionActive_(false),
      senseBlend_(0.75f) {}

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

void VirtualWorldState::applyEmbodiedInput(const EmbodiedInput& input, double deltaTime) {
    std::lock_guard<std::mutex> lock(mtx_);
    const float dt = static_cast<float>(std::max(0.0, deltaTime));
    const float moveSpeed = 2.2f;
    posZ_ += std::clamp(input.moveForward, -1.0f, 1.0f) * moveSpeed * dt;
    posX_ += std::clamp(input.moveRight, -1.0f, 1.0f) * moveSpeed * dt;
    yawDeg_ += std::clamp(input.lookYaw, -180.0f, 180.0f) * dt;
    pitchDeg_ = std::clamp(pitchDeg_ + std::clamp(input.lookPitch, -180.0f, 180.0f) * dt, -89.0f, 89.0f);
    interactionActive_ = input.interact;

    const float senses =
        clamp01((input.vision + input.audition + input.touch + input.smell + input.taste) / 5.0f);
    senseBlend_ = senses;
    setIntensity(std::clamp(0.2f + senses * 0.8f, 0.0f, 1.0f));
}

std::string VirtualWorldState::getAvatarPoseSummary() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return "x=" + std::to_string(posX_) +
           " y=" + std::to_string(posY_) +
           " z=" + std::to_string(posZ_) +
           " yaw=" + std::to_string(yawDeg_) +
           " pitch=" + std::to_string(pitchDeg_) +
           " interact=" + std::string(interactionActive_ ? "1" : "0");
}

float VirtualWorldState::getFiveSenseBlend() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return senseBlend_;
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
