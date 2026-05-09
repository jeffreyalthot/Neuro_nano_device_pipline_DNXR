#pragma once

#include <string>
#include <vector>

namespace dnxr {

class IntentModel {
public:
    /// Classifies user intent from extracted neural features.
    ///
    /// Expected vector layout:
    /// [0] mean, [1] min, [2] max, [3] stdev, [4] absEnergy, [5] trend.
    std::string classifyIntent(const std::vector<float>& features) const;
};

} // namespace dnxr
