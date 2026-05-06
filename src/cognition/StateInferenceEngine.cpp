#include "StateInferenceEngine.h"

namespace dnxr {

std::string StateInferenceEngine::inferState(const std::vector<float>& features) const {
    // This function produces a namespaced cognitive state label from intent classification.
    return "state:" + intent_.classifyIntent(features);
}

} // namespace dnxr
