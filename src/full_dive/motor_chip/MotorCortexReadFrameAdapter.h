#pragma once

#include "full_dive/motor_chip/MotorIntentDecoder.h"
#include "neural_chip/NeuralChipFrame.h"

namespace dnxr {

class MotorCortexReadFrameAdapter {
public:
    static MotorCortexReadFrame fromChipReadFrame(const ChipReadFrame& frame);
};

} // namespace dnxr
