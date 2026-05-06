#pragma once

#include <vector>

namespace dnxr {

class FeatureVectorizer {
public:
    /// Converts a cleaned signal window into a feature vector.
    std::vector<float> vectorize(const std::vector<float>& cleanSignal) const;
};

} // namespace dnxr
