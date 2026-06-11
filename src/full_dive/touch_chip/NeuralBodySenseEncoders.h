#pragma once
#include "neural_chip/NeuralChipFrame.h"
namespace dnxr {
struct AvatarPoseFrame { float head01=0.0f, torso01=0.0f, rightArm01=0.0f, leftArm01=0.0f, rightLeg01=0.0f, leftLeg01=0.0f, handConfidence01=0.0f, footConfidence01=0.0f, ownership01=0.0f, mismatch01=0.0f; };
struct AvatarMotionFrame { float linearAcceleration01=0.0f, angularVelocity01=0.0f, gravityDirection01=0.5f, balanceError01=0.0f, sicknessRisk01=0.0f, visualVestibularMismatch01=0.0f; };
class NeuralProprioceptionEncoder { public: NeuralProprioceptionVector encode(const AvatarPoseFrame& pose) const; };
class NeuralVestibularEncoder { public: NeuralVestibularVector encode(const AvatarMotionFrame& motion) const; };
}
