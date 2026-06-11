#include "full_dive/touch_chip/NeuralBodySenseEncoders.h"
#include <algorithm>
namespace dnxr { namespace { float c(float v){return std::max(0.0f,std::min(1.0f,v));} }
NeuralProprioceptionVector NeuralProprioceptionEncoder::encode(const AvatarPoseFrame& pose) const { NeuralProprioceptionVector out{}; out.headPose01=c(pose.head01); out.torsoPose01=c(pose.torso01); out.rightArmPose01=c(pose.rightArm01); out.leftArmPose01=c(pose.leftArm01); out.rightLegPose01=c(pose.rightLeg01); out.leftLegPose01=c(pose.leftLeg01); out.handPoseConfidence01=c(pose.handConfidence01); out.footPoseConfidence01=c(pose.footConfidence01); out.avatarOwnership01=c(pose.ownership01); out.bodySchemaMismatch01=c(pose.mismatch01); return out; }
NeuralVestibularVector NeuralVestibularEncoder::encode(const AvatarMotionFrame& motion) const { NeuralVestibularVector out{}; out.linearAcceleration01=c(motion.linearAcceleration01); out.angularVelocity01=c(motion.angularVelocity01); out.gravityDirection01=c(motion.gravityDirection01); out.balanceError01=c(motion.balanceError01); out.motionSicknessRisk01=c(motion.sicknessRisk01); out.visualVestibularMismatch01=c(motion.visualVestibularMismatch01); return out; }
}
