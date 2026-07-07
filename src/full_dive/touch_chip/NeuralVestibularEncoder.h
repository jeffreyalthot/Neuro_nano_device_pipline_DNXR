#pragma once

#include "neural_chip/NeuralChipFrame.h"

namespace dnxr {

struct AvatarMotionFrame {
    float linearAcceleration01 = 0.0f;
    float angularVelocity01 = 0.0f;
    float gravityDirection01 = 0.5f;
    float balanceError01 = 0.0f;
    float sicknessRisk01 = 0.0f;
    float visualVestibularMismatch01 = 0.0f;
};

class NeuralVestibularEncoder {
public:
    NeuralVestibularVector encode(const AvatarMotionFrame& motion) const;
};

} // namespace dnxr
