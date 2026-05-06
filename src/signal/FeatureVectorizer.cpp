#include "FeatureVectorizer.h"

#include <algorithm>
#include <cmath>
#include <numeric>

namespace dnxr {

std::vector<float> FeatureVectorizer::vectorize(const std::vector<float>& cleanSignal) const {
    // Produce a deterministic fallback vector when no signal is available.
    if (cleanSignal.empty()) {
        return {0.0F, 0.0F, 0.0F, 0.0F};
    }

    // Compute stable aggregate features suitable for downstream models.
    const auto [minIt, maxIt] = std::minmax_element(cleanSignal.begin(), cleanSignal.end());
    const float sum = std::accumulate(cleanSignal.begin(), cleanSignal.end(), 0.0F);
    const float mean = sum / static_cast<float>(cleanSignal.size());

    float variance = 0.0F;
    for (float value : cleanSignal) {
        const float delta = value - mean;
        variance += delta * delta;
    }
    variance /= static_cast<float>(cleanSignal.size());

    return {mean, *minIt, *maxIt, std::sqrt(variance)};
}

} // namespace dnxr
