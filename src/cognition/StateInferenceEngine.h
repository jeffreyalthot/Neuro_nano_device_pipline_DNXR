#pragma once

#include "IntentModel.h"
#include <string>
#include <vector>

namespace dnxr {

class StateInferenceEngine {
public:
    /// Builds a high-level cognitive state string from feature inputs.
    std::string inferState(const std::vector<float>& features) const;

private:
    IntentModel intent_;
};

} // namespace dnxr
