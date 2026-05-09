#include "IntentModel.h"

namespace dnxr {

std::string IntentModel::classifyIntent(const std::vector<float>& features) const {
    // Guard against invalid vectors and default to safe idle behavior.
    if (features.size() < 6) {
        return "idle";
    }

    const float mean = features[0];
    const float min = features[1];
    const float max = features[2];
    const float stdev = features[3];
    const float absEnergy = features[4];
    const float trend = features[5];

    // Use interpretable thresholds mixing variability, sustained energy, and directional trend.
    if (stdev > 0.25F && max > 0.6F && absEnergy > 0.28F) {
        if (trend > 0.08F) {
            return "deep_immersion_push";
        }
        return "immersive_navigation";
    }
    if (mean < -0.2F || min < -0.7F) {
        return "calm_recovery";
    }
    if (absEnergy < 0.08F && stdev < 0.06F) {
        return "idle";
    }
    return "focus_hold";
}

} // namespace dnxr
