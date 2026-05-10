#pragma once

#include "AdaptationLayer.h"
#include <vector>

namespace dnxr {

class NeuralCalibrationEngine {
public:
    /// Calibrates feature vectors for immersive rendering and stimulation.
    std::vector<float> calibrateForImmersion(const std::vector<float>& features, float userSensitivity) const;

    /// Calibrates spatial-audio control vectors with soft slopes to reduce neural shock risk.
    std::vector<float> calibrateSpatialAudioVectors(const std::vector<float>& audioVector,
                                                    float userSensitivity,
                                                    float comfortGate) const;

private:
    AdaptationLayer adaptation_;
};

} // namespace dnxr
