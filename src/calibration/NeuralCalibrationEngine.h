#pragma once

#include "AdaptationLayer.h"
#include <vector>

namespace dnxr {

class NeuralCalibrationEngine {
public:
    /// Calibrates feature vectors for immersive rendering and stimulation.
    std::vector<float> calibrateForImmersion(const std::vector<float>& features, float userSensitivity) const;

private:
    AdaptationLayer adaptation_;
};

} // namespace dnxr
