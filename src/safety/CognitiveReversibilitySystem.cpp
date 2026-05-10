#include "CognitiveReversibilitySystem.h"

#include <algorithm>
#include <numeric>

namespace dnxr {

bool CognitiveReversibilitySystem::shouldStartReturnToReality(const std::vector<float>& telemetry) const {
    const float avg = telemetry.empty() ? 0.0F : std::accumulate(telemetry.begin(), telemetry.end(), 0.0F) / static_cast<float>(telemetry.size());
    return avg > 0.75F;
}

std::vector<float> CognitiveReversibilitySystem::runDecompression(const std::vector<float>& frame) const {
    std::vector<float> softened = frame;
    for (float& value : softened) {
        value = std::clamp(value * 0.55F, -0.65F, 0.65F);
    }
    return softened;
}

} // namespace dnxr
