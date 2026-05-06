#pragma once

#include "SensoryRenderer.h"
#include <vector>

namespace dnxr {

class ImmersionCore {
public:
    /// Builds an immersive frame from calibrated neural features.
    std::vector<float> buildImmersiveFrame(const std::vector<float>& calibratedSignal);

private:
    SensoryRenderer renderer_;
    VirtualWorldState world_;
};

} // namespace dnxr
