#include "InducedVectorInfluenceEngine.h"

#include <algorithm>
#include <cmath>

namespace dnxr {

namespace {
float clamp01(float v) { return std::clamp(v, 0.0f, 1.0f); }
}

std::vector<float> InducedVectorInfluenceEngine::applyInfluence(const std::vector<float>& neuralVector,
                                                                const std::vector<float>& inducedVector,
                                                                InfluenceTrace& trace) const {
    if (neuralVector.empty()) return {};
    std::vector<float> out = neuralVector;
    if (inducedVector.empty()) {
        trace = InfluenceTrace{};
        return out;
    }

    const std::size_t n = neuralVector.size();
    float accInfluence = 0.0f;
    float accDrift = 0.0f;

    for (std::size_t i = 0; i < n; ++i) {
        const float induced = inducedVector[i % inducedVector.size()];
        const float delta = induced * 0.12f; // soft induced influence gain
        out[i] = neuralVector[i] + delta;
        accInfluence += std::abs(induced);
        accDrift += std::abs(delta);
    }

    trace.inducedInfluence = clamp01((accInfluence / static_cast<float>(n)) * 0.5f);
    trace.decisionDrift = clamp01((accDrift / static_cast<float>(n)) * 0.8f);
    trace.autonomyPreservation = clamp01(1.0f - (trace.decisionDrift * 0.7f + trace.inducedInfluence * 0.3f));
    return out;
}

} // namespace dnxr
