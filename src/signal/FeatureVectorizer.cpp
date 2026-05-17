// FeatureVectorizer.cpp

#include "FeatureVectorizer.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <mutex>

namespace dnxr {

namespace {

std::mutex g_featureMutex;

bool isValid(float v) {
    return std::isfinite(v);
}

float safe(float v) {
    return isValid(v) ? v : 0.0f;
}

} // namespace

FeatureVectorizer::FeatureVectorizer()
    : epsilon_(1e-6f),
      normalizationEnabled_(false),
      lastFeatureEnergy_(0.0f) {
}

std::vector<float> FeatureVectorizer::vectorize(
    const std::vector<float>& cleanSignal) const {

    if (cleanSignal.empty()) {
        return {0.0f, 0.0f, 0.0f, 0.0f};
    }

    std::lock_guard<std::mutex> lock(g_featureMutex);

    std::vector<float> sanitized;
    sanitized.reserve(cleanSignal.size());

    for (float v : cleanSignal) {
        sanitized.push_back(safe(v));
    }

    const auto [minIt, maxIt] =
        std::minmax_element(sanitized.begin(), sanitized.end());

    float sum = std::accumulate(sanitized.begin(), sanitized.end(), 0.0f);
    float mean = sum / static_cast<float>(sanitized.size());

    float variance = 0.0f;
    for (float v : sanitized) {
        float d = v - mean;
        variance += d * d;
    }
    variance /= static_cast<float>(sanitized.size());

    float stdev = std::sqrt(variance + epsilon_);

    std::vector<float> features = {
        mean,
        *minIt,
        *maxIt,
        stdev
    };

    if (normalizationEnabled_) {
        normalize(features);
    }

    lastFeatureEnergy_ = computeEnergy(features);

    return features;
}

float FeatureVectorizer::computeEnergy(
    const std::vector<float>& f) const {

    float e = 0.0f;
    for (float v : f) {
        e += std::abs(v);
    }
    return e;
}

void FeatureVectorizer::normalize(std::vector<float>& f) const {

    float mean = std::accumulate(f.begin(), f.end(), 0.0f)
               / static_cast<float>(f.size());

    float var = 0.0f;
    for (float v : f) {
        var += (v - mean) * (v - mean);
    }
    var /= static_cast<float>(f.size());

    float stddev = std::sqrt(var + epsilon_);

    for (float& v : f) {
        v = (v - mean) / stddev;
    }
}

void FeatureVectorizer::setEpsilon(float eps) {
    epsilon_ = std::max(1e-8f, eps);
}

float FeatureVectorizer::getEpsilon() const {
    return epsilon_;
}

void FeatureVectorizer::enableNormalization(bool enabled) {
    normalizationEnabled_ = enabled;
}

bool FeatureVectorizer::isNormalizationEnabled() const {
    return normalizationEnabled_;
}

float FeatureVectorizer::getLastFeatureEnergy() const {
    return lastFeatureEnergy_;
}

} // namespace dnxr