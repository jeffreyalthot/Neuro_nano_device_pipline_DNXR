#include "NeuroDigitalTwin.h"

#include <algorithm>
#include <numeric>

namespace dnxr {

TwinState NeuroDigitalTwin::update(const std::string& inferredState, const std::vector<float>& telemetry) {
    const float avgTelemetry = telemetry.empty() ? 0.35F : std::accumulate(telemetry.begin(), telemetry.end(), 0.0F) / static_cast<float>(telemetry.size());

    state_.cognitiveLoad = std::clamp(inferredState.find("focused") != std::string::npos ? 0.45F : 0.65F, 0.0F, 1.0F);
    state_.emotionalArousal = std::clamp(inferredState.find("calm") != std::string::npos ? 0.30F : 0.60F, 0.0F, 1.0F);
    state_.physiologicalStress = std::clamp(avgTelemetry, 0.0F, 1.0F);
    return state_;
}

float NeuroDigitalTwin::recommendedIntensity() const {
    const float pressure = (state_.cognitiveLoad + state_.emotionalArousal + state_.physiologicalStress) / 3.0F;
    return std::clamp(1.1F - pressure * 0.6F, 0.45F, 1.0F);
}

} // namespace dnxr
