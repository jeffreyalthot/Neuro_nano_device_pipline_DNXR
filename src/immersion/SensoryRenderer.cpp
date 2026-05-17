#include "SensoryRenderer.h"
#include "../utils/Logger.h"
#include <algorithm>
#include <cmath>

namespace dnxr {

SensoryRenderer::SensoryRenderer()
    : channelGain_(1.0f),
      gamma_(1.8f),
      adaptiveBalance_(true),
      smoothingFactor_(0.75f),
      cognitiveWeight_(1.0f) {}

std::vector<float> SensoryRenderer::renderSensoryFrame(const VirtualWorldState& state) const {
    std::lock_guard<std::mutex> lock(mtx_);

    std::vector<float> out;
    out.reserve(channelCount_);

    const float base = applyGamma(state.getIntensity() * cognitiveWeight_);

    float channels[3] = {
        base * 1.0f,  // canal principal (visuel/lumineux)
        base * 0.85f, // canal secondaire (audio ou proprioception)
        base * 0.65f  // canal tertiaire (haptique/vestibulaire)
    };

    for (float& c : channels) {
        c = applyChannelGain(c);
        c = std::clamp(c, 0.0f, 1.0f);
    }

    if (adaptiveBalance_) {
        balanceChannels(channels);
    }

    out.assign(channels, channels + channelCount_);

    // Émission callback & log
    dispatchFrame(out);

    Logger::info("[SensoryRenderer] Frame rendered | Scene=" + state.getSceneName() +
                 " | base=" + std::to_string(base));
    return out;
}

float SensoryRenderer::applyGamma(float v) const {
    return std::pow(std::clamp(v, 0.0f, 1.0f), gamma_);
}

float SensoryRenderer::applyChannelGain(float v) const {
    return v * channelGain_;
}

void SensoryRenderer::balanceChannels(float channels[3]) const {
    float avg = (channels[0] + channels[1] + channels[2]) / 3.0f;
    for (int i = 0; i < 3; ++i) {
        channels[i] = (smoothingFactor_ * channels[i]) +
                      ((1.0f - smoothingFactor_) * avg);
    }
}

void SensoryRenderer::setChannelGain(float gain) {
    channelGain_ = std::max(0.05f, gain);
}

float SensoryRenderer::getChannelGain() const {
    return channelGain_;
}

void SensoryRenderer::setGamma(float gamma) {
    gamma_ = std::max(0.1f, gamma);
}

float SensoryRenderer::getGamma() const {
    return gamma_;
}

void SensoryRenderer::enableAdaptiveBalance(bool enabled) {
    adaptiveBalance_ = enabled;
}

bool SensoryRenderer::isAdaptiveBalanceEnabled() const {
    return adaptiveBalance_;
}

void SensoryRenderer::applyCognitiveModulation(float stressIndex, float focusIndex) {
    // Influence légère du stress/focus sur l’équilibre sensoriel
    cognitiveWeight_ = computeCognitiveWeight(stressIndex, focusIndex);
}

float SensoryRenderer::computeCognitiveWeight(float stressIndex, float focusIndex) const {
    float modulation = (focusIndex * 0.7f) - (stressIndex * 0.5f);
    modulation = std::clamp(1.0f + modulation, 0.5f, 1.5f);
    return modulation;
}

void SensoryRenderer::setOnFrameRendered(const std::function<void(const std::vector<float>&)>& cb) {
    std::lock_guard<std::mutex> lock(mtx_);
    onFrameRendered_ = cb;
}

void SensoryRenderer::dispatchFrame(const std::vector<float>& frame) const {
    if (onFrameRendered_)
        onFrameRendered_(frame);
}

} // namespace dnxr
