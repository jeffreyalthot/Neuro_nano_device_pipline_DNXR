#pragma once

#include <vector>

namespace dnxr {

class FeatureVectorizer {
public:
    /// Converts a cleaned signal window into a normalized feature vector.
    ///
    /// Feature layout:
    /// [0] mean, [1] min, [2] max, [3] standard deviation,
    /// [4] absolute energy (mean absolute value), [5] trend (last - first sample).
    std::vector<float> vectorize(const std::vector<float>& cleanSignal) const;
};

} // namespace dnxr
