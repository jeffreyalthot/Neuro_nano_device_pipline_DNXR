#pragma once

#include <vector>

namespace dnxr {

class MultiSenseOrchestrator {
public:
    /// Prioritizes sensory channels (audio, visual, haptic, thermal, proprioceptive, vestibular).
    std::vector<float> orchestrate(const std::vector<float>& frame, float intensityScale, bool safetyConstrained) const;
};

} // namespace dnxr
