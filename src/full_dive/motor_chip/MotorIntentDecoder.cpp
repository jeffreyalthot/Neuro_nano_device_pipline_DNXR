#include "full_dive/motor_chip/MotorIntentDecoder.h"
#include <algorithm>
namespace dnxr { namespace { float c(float v){return std::max(0.0f,std::min(1.0f,v));} }
NeuralMotorIntentVector MotorIntentDecoder::decode(const MotorCortexReadFrame& frame) const { NeuralMotorIntentVector out{}; out.moveForward01=c(frame.forward01); out.moveBackward01=c(frame.backward01); out.strafeLeft01=c(frame.left01); out.strafeRight01=c(frame.right01); out.turnYaw01=c(frame.yaw01); out.lookPitch01=c(frame.pitch01); out.rightArmReach01=c(frame.rightReach01); out.leftArmReach01=c(frame.leftReach01); out.rightHandGrip01=c(frame.rightGrip01); out.leftHandGrip01=c(frame.leftGrip01); out.gaitIntent01=c(frame.gait01); out.postureBalance01=c(frame.balance01); out.stopIntent01=c(frame.stop01); out.confidence01=c(frame.confidence01); out.tremorRisk01=c(frame.tremor01); out.spasmRisk01=c(frame.spasm01); out.fatigueRisk01=c(frame.fatigue01); return out; }
}
