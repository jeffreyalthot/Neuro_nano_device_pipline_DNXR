#include "ImmersionCore.h"
#include "GameDesignBridge.h"
#include "../utils/Logger.h"

#include <algorithm>
#include <cmath>
#include <numeric>

namespace dnxr {

namespace {
float clampIntensity(float v) { return std::clamp(v, 0.0f, 1.5f); }

float smoothStep(float current, float target, float factor) {
    return current + (target - current) * factor;
}
} // namespace

ImmersionCore::ImmersionCore()
    : transitionSpeed_(0.12f),
      adaptiveMode_(true),
      lastIntensity_(0.5f),
      frameCounter_(0),
      emotionalMod_(0.5f),
      focusMod_(0.5f) {}

std::vector<float> ImmersionCore::buildImmersiveFrame(const std::vector<float>& calibratedSignal) {
    std::lock_guard<std::mutex> lock(mtx_);

    if (!calibratedSignal.empty()) {
        const float modFactor = std::clamp((emotionalMod_ * 0.6f + focusMod_ * 0.4f), 0.25f, 1.25f);
        const float rawTarget = clampIntensity(calibratedSignal[0] * modFactor);

        if (adaptiveMode_) {
            float blended = smoothStep(world_.getIntensity(), rawTarget, transitionSpeed_);
            world_.setIntensity(blended);
        } else {
            world_.setIntensity(rawTarget);
        }

        lastIntensity_ = world_.getIntensity();

        // Synchronise la scène selon les paramètres cognitifs
        world_.integrateCognitiveInput(emotionalMod_, focusMod_);
    }

    world_.update();

    // Demande au renderer de produire la frame sensorielle
    auto frame = renderer_.renderSensoryFrame(world_);

    // Traitements post-rendu (lissage)
    postProcessFrame(frame);

    ++frameCounter_;

    // Diffusion du résultat
    if (onFrameBuilt_) {
        onFrameBuilt_(frame);
    }

    Logger::info("[ImmersionCore] Frame #" + std::to_string(frameCounter_) +
                 " | intensity=" + std::to_string(lastIntensity_) +
                 " | scene=" + world_.getSceneName());

    return frame;
}

void ImmersionCore::applyCognitiveContext(float emotionalLevel, float focusLevel) {
    emotionalMod_ = std::clamp(emotionalLevel, 0.0f, 1.0f);
    focusMod_ = std::clamp(focusLevel, 0.0f, 1.0f);

    // Transmet la modulation cognitive au renderer
    renderer_.applyCognitiveModulation(emotionalMod_, focusMod_);
}

void ImmersionCore::initializeBaseGameplay() {
    std::lock_guard<std::mutex> lock(mtx_);
    GameDesignBridge bridge;
    bridge.integrateBaseGameplaySystems(world_);
}

void ImmersionCore::applyEmbodiedInput(const VirtualWorldState::EmbodiedInput& input, double deltaTime) {
    std::lock_guard<std::mutex> lock(mtx_);
    world_.applyEmbodiedInput(input, deltaTime);
}

std::string ImmersionCore::getAvatarPoseSummary() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return world_.getAvatarPoseSummary();
}

void ImmersionCore::setTransitionSpeed(float speed) {
    transitionSpeed_ = std::clamp(speed, 0.01f, 1.0f);
}

float ImmersionCore::getTransitionSpeed() const { return transitionSpeed_; }

void ImmersionCore::enableAdaptiveMode(bool enabled) { adaptiveMode_ = enabled; }
bool ImmersionCore::isAdaptiveModeEnabled() const { return adaptiveMode_; }

float ImmersionCore::getLastIntensity() const { return lastIntensity_; }
uint64_t ImmersionCore::getFrameCount() const { return frameCounter_; }

void ImmersionCore::setOnFrameBuilt(const std::function<void(const std::vector<float>&)>& cb) {
    std::lock_guard<std::mutex> lock(mtx_);
    onFrameBuilt_ = cb;
}

void ImmersionCore::postProcessFrame(std::vector<float>& frame) const {
    if (frame.empty()) return;

    float avg = std::accumulate(frame.begin(), frame.end(), 0.0f) / static_cast<float>(frame.size());
    for (float& v : frame) {
        v = (0.85f * v) + (0.15f * avg);
    }
}

} // namespace dnxr
