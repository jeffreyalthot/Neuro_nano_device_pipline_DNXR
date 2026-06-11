#include "full_dive/motor_chip/AvatarBalanceController.h"
#include <algorithm>
namespace dnxr { float AvatarBalanceController::clamp01(float v){return std::max(0.0f,std::min(1.0f,v));} AvatarBalanceState AvatarBalanceController::stabilize(const NeuralMotorIntentVector& i,const AvatarGaitState& g) const { AvatarBalanceState s; s.freezeRig=i.spasmRisk01>0.5f; s.centerOfMassStability01=s.freezeRig?1.0f:clamp01(i.postureBalance01*(1.0f-g.speed01*0.25f)); s.damping01=clamp01(0.4f+i.tremorRisk01*0.6f+(1.0f-i.confidence01)*0.3f); return s;} }
