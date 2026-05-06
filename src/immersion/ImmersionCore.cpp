#include "ImmersionCore.h"

#include <algorithm>

namespace dnxr {

std::vector<float> ImmersionCore::buildImmersiveFrame(const std::vector<float>& calibratedSignal) {
    // This function maps calibrated neural intensity into bounded world-state rendering parameters.
    if (!calibratedSignal.empty()) {
        world_.intensity = std::clamp(calibratedSignal[0], 0.0F, 1.5F);
    }

    return renderer_.renderSensoryFrame(world_);
}

} // namespace dnxr
