#include "HostComputerBridge.h"

#include <cmath>
#include <algorithm>

namespace dnxr {

std::vector<float> HostComputerBridge::uplink(const std::vector<float>& signal) const {
    // This function simulates host uplink normalization to keep values within transport-safe bounds.
    std::vector<float> normalized(signal.size(), 0.0F);
    std::transform(signal.begin(), signal.end(), normalized.begin(), [](float sample) {
        return std::clamp(sample, -1.0F, 1.0F);
    });
    return normalized;
}

std::vector<float> HostComputerBridge::downlink(const std::vector<float>& command) const {
    // This function simulates command downlink quantization to improve actuator stability.
    std::vector<float> quantized(command.size(), 0.0F);
    std::transform(command.begin(), command.end(), quantized.begin(), [](float value) {
        constexpr float kStep = 0.05F;
        return std::round(value / kStep) * kStep;
    });
    return quantized;
}

} // namespace dnxr
