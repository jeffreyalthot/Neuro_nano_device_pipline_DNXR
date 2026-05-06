#include "NeuralCalibrationEngine.h"

namespace dnxr {

std::vector<float> NeuralCalibrationEngine::calibrateForImmersion(
    const std::vector<float>& features,
    float userSensitivity) const {
    // This function applies user-specific adaptation gains to immersion feature vectors.
    return adaptation_.applyUserProfile(features, userSensitivity);
}

} // namespace dnxr
