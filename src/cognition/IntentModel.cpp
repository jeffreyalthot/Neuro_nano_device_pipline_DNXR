#include "IntentModel.h"

namespace dnxr {

std::string IntentModel::classifyIntent(const std::vector<float>& features) const {
    // Guard against invalid vectors and default to safe idle behavior.
    if (features.size() < 4) {
        return "idle";
    }

    const float mean = features[0];
    const float min = features[1];
    const float max = features[2];
    const float stdev = features[3];

    // Use simple, interpretable thresholds for conceptual intent classes.
    if (stdev > 0.25F && max > 0.6F) {
        return "immersive_navigation";
    }
    if (mean < -0.2F || min < -0.7F) {
        return "calm_recovery";
    }
    return "focus_hold";
}

} // namespace dnxr
