#pragma once

#include <vector>

namespace dnxr {

struct InfluenceTrace {
    float inducedInfluence = 0.0f;
    float decisionDrift = 0.0f;
    float autonomyPreservation = 1.0f;
};

class InducedVectorInfluenceEngine {
public:
    std::vector<float> applyInfluence(const std::vector<float>& neuralVector,
                                      const std::vector<float>& inducedVector,
                                      InfluenceTrace& trace) const;
};

} // namespace dnxr
