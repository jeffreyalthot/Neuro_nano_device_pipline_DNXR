#pragma once

#include <vector>

namespace dnxr {

class CognitiveReversibilitySystem {
public:
    bool shouldStartReturnToReality(const std::vector<float>& telemetry) const;
    std::vector<float> runDecompression(const std::vector<float>& frame) const;
};

} // namespace dnxr
