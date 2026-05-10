#include "MultiSenseOrchestrator.h"

#include <algorithm>

namespace dnxr {

std::vector<float> MultiSenseOrchestrator::orchestrate(const std::vector<float>& frame, float intensityScale, bool safetyConstrained) const {
    std::vector<float> routed = frame;
    for (std::size_t i = 0; i < routed.size(); ++i) {
        const float channelBias = (i % 6 == 0) ? 1.0F : (i % 6 == 1) ? 0.95F : (i % 6 == 2) ? 0.92F : (i % 6 == 3) ? 0.78F : (i % 6 == 4) ? 0.85F : 0.74F;
        const float safety = safetyConstrained ? 0.75F : 1.0F;
        routed[i] = std::clamp(routed[i] * intensityScale * channelBias * safety, -1.0F, 1.0F);
    }
    return routed;
}

} // namespace dnxr
