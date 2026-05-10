#pragma once

#include <string>
#include <vector>

namespace dnxr {

struct TwinState {
    float cognitiveLoad;
    float emotionalArousal;
    float physiologicalStress;
};

class NeuroDigitalTwin {
public:
    /// Builds a living user model from inferred state and wearable telemetry.
    TwinState update(const std::string& inferredState, const std::vector<float>& telemetry);

    /// Computes bounded sensory intensity to avoid overload and cybersickness.
    float recommendedIntensity() const;

private:
    TwinState state_{0.3F, 0.3F, 0.3F};
};

} // namespace dnxr
