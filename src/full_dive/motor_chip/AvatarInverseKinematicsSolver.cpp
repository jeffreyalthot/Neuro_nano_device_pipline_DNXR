#include "full_dive/motor_chip/AvatarInverseKinematicsSolver.h"
#include <algorithm>
namespace dnxr { void AvatarInverseKinematicsSolver::solve(const NeuralMotorIntentVector& intent,const AvatarBalanceState& balance,AvatarRigCommandFrame& frame) const { for(auto& j:frame.joints){ j.damping01=std::max(j.damping01,balance.damping01); if(balance.freezeRig||intent.stopIntent01>0.5f){ j.stiffness01=0.0f; j.damping01=1.0f; j.confidence01=std::min(j.confidence01,0.2f); } } frame.centerOfMassStability01=balance.centerOfMassStability01; } }
