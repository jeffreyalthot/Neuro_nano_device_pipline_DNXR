#pragma once
#include "neural_chip/NeuralChipFrame.h"
namespace dnxr {
struct MotorCortexReadFrame { float forward01=0.0f, backward01=0.0f, left01=0.0f, right01=0.0f, yaw01=0.0f, pitch01=0.0f, rightReach01=0.0f, leftReach01=0.0f, rightGrip01=0.0f, leftGrip01=0.0f, gait01=0.0f, balance01=0.0f, stop01=1.0f, confidence01=0.0f, tremor01=0.0f, spasm01=0.0f, fatigue01=0.0f; };
class MotorIntentDecoder { public: NeuralMotorIntentVector decode(const MotorCortexReadFrame& frame) const; };
}
