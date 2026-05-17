#include "StateInferenceEngine.h"
#include <algorithm>
#include <numeric>
#include <cmath>

namespace dnxr {

StateInferenceEngine::StateInferenceEngine()
    : confidenceThreshold_(0.55f),
      smoothingFactor_(0.8f),
      contextWindowSize_(5) {}

std::string StateInferenceEngine::inferState(
    const std::vector<float>& features,
    double timestamp)
{
    std::lock_guard<std::mutex> lock(mtx_);

    if (features.empty()) {
        emitState("state:undefined", timestamp);
        return "state:undefined";
    }

    // 1. Extraction de contexte
    std::vector<float> context = extractContext(features);

    // 2. Stabilisation temporelle (anti‑bruit cognitif)
    smoothContext(context);

    // 3. Classification d’intention (modèle interne temps réel)
    IntentResult result = intent_.classifyIntent(context);

    // 4. Vérification du seuil de confiance
    std::string label;
    if (result.confidence < confidenceThreshold_) {
        label = "state:uncertain";
    } else {
        // 5. Construction sémantique enrichie
        label = buildStateLabel(result);
    }

    // 6. Emission vers le pipeline cognitif global
    emitState(label, timestamp);

    return label;
}

void StateInferenceEngine::setConfidenceThreshold(float threshold) {
    confidenceThreshold_ = std::clamp(threshold, 0.0f, 1.0f);
}

float StateInferenceEngine::getConfidenceThreshold() const {
    return confidenceThreshold_;
}

void StateInferenceEngine::setSmoothingFactor(float factor) {
    smoothingFactor_ = std::clamp(factor, 0.0f, 1.0f);
}

float StateInferenceEngine::getSmoothingFactor() const {
    return smoothingFactor_;
}

void StateInferenceEngine::setContextWindowSize(size_t size) {
    contextWindowSize_ = std::max<size_t>(1, size);
}

size_t StateInferenceEngine::getContextWindowSize() const {
    return contextWindowSize_;
}

void StateInferenceEngine::setOnStateInferred(
    const std::function<void(const std::string&, double)>& cb)
{
    onStateInferred_ = cb;
}

// --- Fonctions internes ---

std::vector<float> StateInferenceEngine::extractContext(
    const std::vector<float>& input) const
{
    if (input.size() <= contextWindowSize_) {
        return input;
    }

    std::vector<float> out;
    out.reserve(input.size() - contextWindowSize_ + 1);

    for (size_t i = 0; i < input.size() - contextWindowSize_ + 1; ++i) {
        float accum = 0.0f;
        for (size_t j = 0; j < contextWindowSize_; ++j) {
            accum += input[i + j];
        }
        out.push_back(accum / static_cast<float>(contextWindowSize_));
    }

    return out;
}

void StateInferenceEngine::smoothContext(std::vector<float>& data) const {
    if (data.empty()) return;

    float prev = data.front();
    for (float& v : data) {
        v = (smoothingFactor_ * prev) + ((1.0f - smoothingFactor_) * v);
        prev = v;
    }
}

std::string StateInferenceEngine::buildStateLabel(const IntentResult& result) const {
    std::string label = "state:" + result.intent;

    if (result.confidence > 0.9f)
        label += ":high_conf";
    else if (result.confidence > 0.7f)
        label += ":mid_conf";
    else
        label += ":low_conf";

    return label;
}

void StateInferenceEngine::emitState(const std::string& state, double timestamp) const {
    if (onStateInferred_) {
        onStateInferred_(state, timestamp);
    }
}

} // namespace dnxr
