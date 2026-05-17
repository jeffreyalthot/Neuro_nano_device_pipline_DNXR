// SignalStreamProcessor.cpp

#include "SignalStreamProcessor.h"

#include <algorithm>
#include <cmath>
#include <numeric>
#include <vector>
#include <mutex>

namespace dnxr {

namespace {

constexpr float kMinAmplitude = -1.0f;
constexpr float kMaxAmplitude =  1.0f;

std::mutex g_signalMutex;

float safeClamp(float v) {
    if (!std::isfinite(v)) return 0.0f;
    return std::clamp(v, kMinAmplitude, kMaxAmplitude);
}

} // namespace

SignalStreamProcessor::SignalStreamProcessor()
    : smoothingWindow_(3),
      adaptiveMode_(true),
      lastNoiseEstimate_(0.0f) {
}

std::vector<float> SignalStreamProcessor::denoise(
    const std::vector<float>& raw) const {

    if (raw.empty()) {
        return {};
    }

    std::lock_guard<std::mutex> lock(g_signalMutex);

    std::vector<float> clamped;
    clamped.reserve(raw.size());

    // Phase 1: clamp + sanitize
    for (float v : raw) {
        clamped.push_back(safeClamp(v));
    }

    // Phase 2: moving average smoothing
    std::vector<float> smoothed;
    smoothed.reserve(clamped.size());

    for (size_t i = 0; i < clamped.size(); ++i) {

        float left   = (i == 0) ? clamped[i] : clamped[i - 1];
        float center = clamped[i];
        float right  = (i + 1 < clamped.size()) ? clamped[i + 1] : clamped[i];

        float value = (left + center + right) / 3.0f;

        if (adaptiveMode_) {
            value = applyAdaptiveFilter(value);
        }

        smoothed.push_back(value);
    }

    lastNoiseEstimate_ = estimateNoise(smoothed);

    return smoothed;
}

float SignalStreamProcessor::estimateNoise(
    const std::vector<float>& data) const {

    if (data.empty()) {
        return 0.0f;
    }

    float mean = std::accumulate(data.begin(), data.end(), 0.0f)
               / static_cast<float>(data.size());

    float var = 0.0f;
    for (float v : data) {
        var += (v - mean) * (v - mean);
    }
    var /= static_cast<float>(data.size());

    return std::sqrt(var);
}

float SignalStreamProcessor::applyAdaptiveFilter(float v) const {
    // Compression douce des fluctuations extrêmes
    return std::tanh(v * 1.2f);
}

void SignalStreamProcessor::setSmoothingWindow(size_t window) {
    smoothingWindow_ = std::max<size_t>(1, window);
}

size_t SignalStreamProcessor::getSmoothingWindow() const {
    return smoothingWindow_;
}

void SignalStreamProcessor::enableAdaptiveMode(bool enabled) {
    adaptiveMode_ = enabled;
}

bool SignalStreamProcessor::isAdaptiveModeEnabled() const {
    return adaptiveMode_;
}

float SignalStreamProcessor::getLastNoiseEstimate() const {
    return lastNoiseEstimate_;
}

} // namespace dnxr