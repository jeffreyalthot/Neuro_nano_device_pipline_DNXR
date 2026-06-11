#pragma once
#include "full_dive/motor_chip/AvatarGaitPhaseController.h"
namespace dnxr { struct AvatarBalanceState { float centerOfMassStability01=0.0f; float damping01=0.5f; bool freezeRig=false; }; class AvatarBalanceController { public: AvatarBalanceState stabilize(const NeuralMotorIntentVector& intent, const AvatarGaitState& gait) const; private: static float clamp01(float value); }; }
