#include "AdaptationLayer.h"

#include <algorithm>

namespace dnxr {

std::vector<float> AdaptationLayer::applyUserProfile(const std::vector<float>& vec, float gain) const {
    // This function scales and clamps each feature according to the active sensitivity gain.
    std::vector<float> out = vec;
    for (float& value : out) {
        value = std::clamp(value * gain, -2.0F, 2.0F);
    }
    return out;
}

} // namespace dnxr
