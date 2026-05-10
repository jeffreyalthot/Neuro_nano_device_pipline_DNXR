#include "NeuralCalibrationEngine.h"

#include <algorithm>

namespace dnxr {

std::vector<float> NeuralCalibrationEngine::calibrateForImmersion(
    const std::vector<float>& features,
    float userSensitivity) const {
    // This function applies user-specific adaptation gains to immersion feature vectors.
    return adaptation_.applyUserProfile(features, userSensitivity);
}

std::vector<float> NeuralCalibrationEngine::calibrateSpatialAudioVectors(const std::vector<float>& audioVector,
                                                                         float userSensitivity,
                                                                         float comfortGate) const {
    // This function applies gentle calibration gain and clamps spatial audio vectors for comfort.
    std::vector<float> adjusted = adaptation_.applyUserProfile(audioVector, userSensitivity * 0.75F);
    const float gate = std::clamp(comfortGate, 0.2F, 1.0F);
    for (float& sample : adjusted) {
        sample = std::clamp(sample * gate, -0.85F, 0.85F);
    }
    return adjusted;
}

} // namespace dnxr
