#include "NeuralCalibrationEngine.h"

#include <algorithm>
#include <cmath>
#include <numeric>

namespace dnxr {

NeuralCalibrationEngine::NeuralCalibrationEngine()
    : immersionIntensity_(1.0f),
      adaptiveNoiseReduction_(true),
      emotionalFeedbackGain_(0.5f),
      stabilizationFactor_(0.85f) {}

std::vector<float> NeuralCalibrationEngine::calibrateForImmersion(
    const std::vector<float>& features,
    float userSensitivity,
    double timestamp)
{
    std::lock_guard<std::mutex> lock(mtx_);

    if (features.empty()) {
        return {};
    }

    // 1. Adaptation individuelle et profil utilisateur
    std::vector<float> adapted = adaptation_.processInputStream(features, userSensitivity, timestamp);

    // 2. Stabilisation neuronale temporelle
    stabilizeSignal(adapted);

    // 3. Amplification immersive (renforcement perceptuel)
    applyImmersionAmplification(adapted);

    // 4. Réduction de bruit adaptative (anti-oscillation cognitive)
    if (adaptiveNoiseReduction_) {
        reduceNeuralNoise(adapted);
    }

    // 5. Compensation émotionnelle dynamique
    applyEmotionalCompensation(adapted);

    // 6. Émission du signal calibré
    emitCalibrated(adapted, timestamp);

    return adapted;
}

void NeuralCalibrationEngine::setImmersionIntensity(float intensity) {
    immersionIntensity_ = std::clamp(intensity, 0.1f, 5.0f);
}

float NeuralCalibrationEngine::getImmersionIntensity() const {
    return immersionIntensity_;
}

void NeuralCalibrationEngine::enableAdaptiveNoiseReduction(bool enabled) {
    adaptiveNoiseReduction_ = enabled;
}

bool NeuralCalibrationEngine::isAdaptiveNoiseReductionEnabled() const {
    return adaptiveNoiseReduction_;
}

void NeuralCalibrationEngine::setEmotionalFeedbackGain(float gain) {
    emotionalFeedbackGain_ = std::clamp(gain, 0.0f, 2.0f);
}

float NeuralCalibrationEngine::getEmotionalFeedbackGain() const {
    return emotionalFeedbackGain_;
}

void NeuralCalibrationEngine::setStabilizationFactor(float factor) {
    stabilizationFactor_ = std::clamp(factor, 0.0f, 1.0f);
}

float NeuralCalibrationEngine::getStabilizationFactor() const {
    return stabilizationFactor_;
}

float NeuralCalibrationEngine::computeImmersionScore(const std::vector<float>& calibrated) const {
    if (calibrated.empty()) return 0.0f;

    float energy = std::accumulate(
        calibrated.begin(),
        calibrated.end(),
        0.0f,
        [](float acc, float v) { return acc + std::abs(v); });

    return energy / static_cast<float>(calibrated.size());
}

// --- Procédures internes ---

void NeuralCalibrationEngine::applyImmersionAmplification(std::vector<float>& signal) const {
    for (float& value : signal) {
        value *= immersionIntensity_;
        // Compression douce sensorielle pour éviter la saturation physiologique
        value = std::tanh(value * 0.95f);
    }
}

void NeuralCalibrationEngine::reduceNeuralNoise(std::vector<float>& signal) const {
    if (signal.size() < 3) return;

    std::vector<float> filtered(signal.size());
    filtered[0] = signal.front();
    filtered.back() = signal.back();

    for (size_t i = 1; i + 1 < signal.size(); ++i)
        filtered[i] = (signal[i - 1] + signal[i] + signal[i + 1]) / 3.0f;

    signal.swap(filtered);
}

void NeuralCalibrationEngine::stabilizeSignal(std::vector<float>& signal) const {
    float prev = 0.0f;
    for (float& value : signal) {
        value = stabilizationFactor_ * prev +
                (1.0f - stabilizationFactor_) * value;
        prev = value;
    }
}

void NeuralCalibrationEngine::applyEmotionalCompensation(std::vector<float>& signal) const {
    for (float& value : signal) {
        float mod = 1.0f + std::sin(value * 3.1415926f) * emotionalFeedbackGain_;
        value *= mod;
    }
}

void NeuralCalibrationEngine::setOnCalibratedOutput(
    const std::function<void(const std::vector<float>&, double)>& cb)
{
    onCalibratedOutput_ = cb;
}

void NeuralCalibrationEngine::emitCalibrated(
    const std::vector<float>& signal,
    double timestamp) const
{
    if (onCalibratedOutput_)
        onCalibratedOutput_(signal, timestamp);
}

} // namespace dnxr
