#pragma once

#include "SpatialAudioEngine.h"
#include "VirtualWorldState.h"
#include <vector>

namespace dnxr {

class SensoryRenderer {
public:
    /// Converts world state into an XR-ready multi-channel sensory frame.
    std::vector<float> renderSensoryFrame(const VirtualWorldState& state) const;

private:
    SpatialAudioEngine audio_;
};

} // namespace dnxr
