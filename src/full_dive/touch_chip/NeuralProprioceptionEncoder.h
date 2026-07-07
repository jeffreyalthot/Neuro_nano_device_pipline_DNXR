#pragma once

#include "neural_chip/NeuralChipFrame.h"

namespace dnxr {

struct AvatarPoseFrame {
    float head01 = 0.0f;
    float torso01 = 0.0f;
    float rightArm01 = 0.0f;
    float leftArm01 = 0.0f;
    float rightLeg01 = 0.0f;
    float leftLeg01 = 0.0f;
    float handConfidence01 = 0.0f;
    float footConfidence01 = 0.0f;
    float ownership01 = 0.0f;
    float mismatch01 = 0.0f;
};

class NeuralProprioceptionEncoder {
public:
    NeuralProprioceptionVector encode(const AvatarPoseFrame& pose) const;
};

} // namespace dnxr
