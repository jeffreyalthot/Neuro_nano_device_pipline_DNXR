#include "SignalStreamProcessor.h"

#include <algorithm>

namespace dnxr {

std::vector<float> SignalStreamProcessor::denoise(const std::vector<float>& raw) const {
    // Return early for empty input to avoid unnecessary allocations.
    if (raw.empty()) {
        return {};
    }

    // Clamp extreme values and smooth with a 3-point moving average.
    constexpr float kMinAmplitude = -1.0F;
    constexpr float kMaxAmplitude = 1.0F;

    std::vector<float> clamped(raw.size());
    std::transform(raw.begin(), raw.end(), clamped.begin(), [kMinAmplitude, kMaxAmplitude](float sample) {
        return std::clamp(sample, kMinAmplitude, kMaxAmplitude);
    });

    std::vector<float> smoothed(clamped.size(), 0.0F);
    for (std::size_t i = 0; i < clamped.size(); ++i) {
        const float left = (i == 0) ? clamped[i] : clamped[i - 1];
        const float center = clamped[i];
        const float right = (i + 1 >= clamped.size()) ? clamped[i] : clamped[i + 1];
        smoothed[i] = (left + center + right) / 3.0F;
    }

    return smoothed;
}

} // namespace dnxr
