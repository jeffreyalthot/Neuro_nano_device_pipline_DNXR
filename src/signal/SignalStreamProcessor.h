// SignalStreamProcessor.h

#pragma once

#include <vector>
#include <cstddef>

namespace dnxr {

class SignalStreamProcessor {
public:
    SignalStreamProcessor();

    /// Denoising + smoothing du flux neuronal brut.
    std::vector<float> denoise(const std::vector<float>& raw) const;

    void setSmoothingWindow(size_t window);
    size_t getSmoothingWindow() const;

    void enableAdaptiveMode(bool enabled);
    bool isAdaptiveModeEnabled() const;

    float getLastNoiseEstimate() const;

private:
    float estimateNoise(const std::vector<float>& data) const;
    float applyAdaptiveFilter(float v) const;

private:
    size_t smoothingWindow_;
    bool adaptiveMode_;
    mutable float lastNoiseEstimate_;
};

} // namespace dnxr