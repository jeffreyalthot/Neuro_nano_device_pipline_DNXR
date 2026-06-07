#pragma once

#include "../full_dive/BodySchemaEngine.h"
#include "../hardware/NeuralChipHAL.h"

#include <vector>

namespace dnxr {

class PerceivedBodyPoseEstimator {
public:
    BodyPoseState estimate(const NeuralChipFrame& neuralFrame,
                           const std::vector<float>& headsetSignals,
                           const std::vector<float>& bodySensors,
                           const std::vector<float>& proprioceptiveModel) const;
    static bool isComplete(const BodyPoseState& pose);
};

} // namespace dnxr
