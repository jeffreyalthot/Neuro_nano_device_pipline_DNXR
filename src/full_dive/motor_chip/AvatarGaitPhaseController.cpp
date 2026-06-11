#include "full_dive/motor_chip/AvatarGaitPhaseController.h"
#include <algorithm>
namespace dnxr { float AvatarGaitPhaseController::clamp01(float v){return std::max(0.0f,std::min(1.0f,v));} AvatarGaitState AvatarGaitPhaseController::update(const NeuralMotorIntentVector& i) const { AvatarGaitState s; if(i.stopIntent01>0.5f||i.spasmRisk01>0.5f){s.footLock01=1.0f; return s;} s.speed01=clamp01(i.gaitIntent01+i.moveForward01+i.moveBackward01); s.phase01=s.speed01; s.footLock01=1.0f-s.speed01; return s;} }
