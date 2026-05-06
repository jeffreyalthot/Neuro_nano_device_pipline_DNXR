#pragma once

#include <string>
#include <vector>

namespace dnxr {

class IntentModel {
public:
    /// Classifies user intent from extracted neural features.
    std::string classifyIntent(const std::vector<float>& features) const;
};

} // namespace dnxr
