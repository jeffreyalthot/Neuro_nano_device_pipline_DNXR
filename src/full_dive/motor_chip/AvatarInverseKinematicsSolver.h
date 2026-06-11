#pragma once
#include "full_dive/motor_chip/AvatarKinematicRigController.h"
#include "full_dive/motor_chip/AvatarBalanceController.h"
namespace dnxr { class AvatarInverseKinematicsSolver { public: void solve(const NeuralMotorIntentVector& intent, const AvatarBalanceState& balance, AvatarRigCommandFrame& frame) const; }; }
