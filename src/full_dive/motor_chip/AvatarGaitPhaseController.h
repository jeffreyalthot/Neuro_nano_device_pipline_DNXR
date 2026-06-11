#pragma once
#include "neural_chip/NeuralChipFrame.h"
namespace dnxr { struct AvatarGaitState { float phase01=0.0f; float speed01=0.0f; float footLock01=1.0f; }; class AvatarGaitPhaseController { public: AvatarGaitState update(const NeuralMotorIntentVector& intent) const; private: static float clamp01(float value); }; }
