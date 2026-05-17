#include "AdaptationLayer.h"

#include <algorithm>
#include <cmath>
#include <numeric>

namespace dnxr {

AdaptationLayer::AdaptationLayer()
    : baseGain_(1.0f),
      clampMin_(-1.5f),
      clampMax_(1.5f),
      nonlinearMode_(true),
      normalizationEnabled_(true) {}

std::vector<float> AdaptationLayer::processInputStream(
    const std::vector<float>& input,
    float userGain,
    double timestamp)
{
    std::lock_guard<std::mutex> lock(mtx_);

    if (input.empty()) {
        return {};
    }

    float effectiveGain = computeEffectiveGain(userGain);
    std::vector<float> output;
    output.reserve(input.size());

    // --- Étape 1 : application du gain et modèle sensori‑cognitif ---
    for (float v : input) {
        float adjusted = v * effectiveGain;
        if (nonlinearMode_) {
            adjusted = nonlinearTransform(adjusted);
        }
        output.push_back(adjusted);
    }

    // --- Étape 2 : normalisation dynamique ---
    if (normalizationEnabled_) {
        normalize(output);
    }

    // --- Étape 3 : filtrage de sécurité (clamp) ---
    clampOutput(output);

    // --- Étape 4 : émission vers le pipeline supérieur (XR/Neural bridge) ---
    emitAdapted(output, timestamp);

    return output;
}

void AdaptationLayer::setBaseGain(float gain) {
    baseGain_ = std::max(0.01f, gain);
}

float AdaptationLayer::getBaseGain() const {
    return baseGain_;
}

void AdaptationLayer::setClampRange(float minVal, float maxVal) {
    if (minVal < maxVal) {
        clampMin_ = minVal;
        clampMax_ = maxVal;
    }
}

void AdaptationLayer::enableNonlinearMode(bool enabled) {
    nonlinearMode_ = enabled;
}

void AdaptationLayer::enableNormalization(bool enabled) {
    normalizationEnabled_ = enabled;
}

void AdaptationLayer::setOnAdaptedOutput(
    const std::function<void(const std::vector<float>&, double)>& cb)
{
    onAdaptedOutput_ = cb;
}

float AdaptationLayer::computeEffectiveGain(float userGain) const {
    return baseGain_ * userGain;
}

float AdaptationLayer::nonlinearTransform(float x) const {
    // Réponse inspirée des fonctions perceptuelles naturelles
    // Applique un lissage énergétiquement cohérent (évite saturation neuronale)
    return std::tanh(x) + 0.12f * std::sin(2.2f * x);
}

void AdaptationLayer::normalize(std::vector<float>& data) const {
    float mean = std::accumulate(data.begin(), data.end(), 0.0f)
                 / static_cast<float>(data.size());

    float var = 0.0f;
    for (float v : data)
        var += (v - mean) * (v - mean);
    var /= static_cast<float>(data.size());

    float stddev = std::sqrt(var);
    if (stddev < 1e-6f)
        return;

    for (float& v : data)
        v = (v - mean) / stddev;
}

void AdaptationLayer::clampOutput(std::vector<float>& data) const {
    for (float& v : data)
        v = std::clamp(v, clampMin_, clampMax_);
}

void AdaptationLayer::emitAdapted(
    const std::vector<float>& data,
    double timestamp) const
{
    if (onAdaptedOutput_) {
        onAdaptedOutput_(data, timestamp);
    }
}

} // namespace dnxr
