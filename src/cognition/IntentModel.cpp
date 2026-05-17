#include "IntentModel.h"
#include <algorithm>
#include <cmath>
#include <numeric>

namespace dnxr {

IntentModel::IntentModel()
    : volatilityThreshold_(0.25f),
      immersionThreshold_(0.6f),
      calmMeanThreshold_(-0.2f),
      calmMinThreshold_(-0.7f),
      smoothingEnabled_(true) {}

IntentResult IntentModel::classifyIntent(const std::vector<float>& features) const {
    std::lock_guard<std::mutex> lock(mtx_);

    if (features.size() < 4) {
        IntentResult idle{"idle", 0.0f, 0.0f};
        emitIntent(idle);
        return idle;
    }

    // --- 1 : extraction statistique robuste ---
    FeatureStats stats = computeStats(features);

    // --- 2 : calcul de scores pondérés selon l’état ---
    float immersiveScore = computeImmersiveScore(stats);
    float calmScore = computeCalmScore(stats);
    float focusScore = computeFocusScore(stats);

    // --- 3 : décision d’intention dominante ---
    IntentResult result = selectBestIntent(immersiveScore, calmScore, focusScore);

    // --- 4 : calibration de confiance contextuelle ---
    result.confidence = computeConfidence(stats, result.intent);

    // --- 5 : diffusion éventuelle ---
    emitIntent(result);

    return result;
}

void IntentModel::setContextAdaptation(float volatilityThr, float immersionThr) {
    std::lock_guard<std::mutex> lock(mtx_);
    volatilityThreshold_ = std::clamp(volatilityThr, 0.05f, 2.0f);
    immersionThreshold_ = std::clamp(immersionThr, 0.1f, 3.0f);
}

void IntentModel::enableSmoothing(bool enabled) {
    smoothingEnabled_ = enabled;
}

void IntentModel::setOnIntentUpdated(const std::function<void(const IntentResult&)>& cb) {
    onIntentUpdated_ = cb;
}

// --- Calculs internes ---

FeatureStats IntentModel::computeStats(const std::vector<float>& f) const {
    FeatureStats s{};
    if (f.empty()) return s;

    s.mean = std::accumulate(f.begin(), f.end(), 0.0f) / static_cast<float>(f.size());
    s.min = *std::min_element(f.begin(), f.end());
    s.max = *std::max_element(f.begin(), f.end());

    float variance = 0.0f;
    for (float v : f) variance += (v - s.mean) * (v - s.mean);
    s.stdev = std::sqrt(variance / static_cast<float>(f.size()));

    // Si le bruit est fort, applique un amortissement global (smoothing optionnel)
    if (smoothingEnabled_ && s.stdev > 1.5f)
        s.stdev = 1.5f;

    return s;
}

float IntentModel::computeImmersiveScore(const FeatureStats& s) const {
    float volatility = std::clamp(s.stdev, 0.001f, 10.0f);
    float intensity = std::clamp(s.max, -2.0f, 2.0f);
    return (volatility / volatilityThreshold_) * 0.6f +
           (intensity / immersionThreshold_) * 0.4f;
}

float IntentModel::computeCalmScore(const FeatureStats& s) const {
    float calmMean = std::max(0.0f, calmMeanThreshold_ - s.mean);
    float calmMin = std::max(0.0f, calmMinThreshold_ - s.min);
    return (calmMean + calmMin) * 0.7f;
}

float IntentModel::computeFocusScore(const FeatureStats& s) const {
    float dispersion = std::max(0.001f, s.max - s.min);
    float stability = 1.0f / (1.0f + dispersion);
    return std::abs(s.mean) * stability * 2.0f;
}

IntentResult IntentModel::selectBestIntent(float immersive, float calm, float focus) const {
    IntentResult r;
    if (immersive > calm && immersive > focus) {
        r.intent = "immersive_navigation";
        r.score = immersive;
    } else if (calm > focus) {
        r.intent = "calm_recovery";
        r.score = calm;
    } else {
        r.intent = "focus_hold";
        r.score = focus;
    }
    return r;
}

float IntentModel::computeConfidence(const FeatureStats& stats, const std::string& intent) const {
    float activity = std::abs(stats.mean) + stats.stdev * 0.5f;
    if (intent == "immersive_navigation")
        return std::min(1.0f, activity * 1.1f);
    if (intent == "focus_hold")
        return std::min(1.0f, activity);
    return std::clamp(activity * 0.9f, 0.0f, 1.0f);
}

void IntentModel::emitIntent(const IntentResult& res) const {
    if (onIntentUpdated_) onIntentUpdated_(res);
}

} // namespace dnxr
