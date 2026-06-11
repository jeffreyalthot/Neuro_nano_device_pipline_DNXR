#include "full_dive/motor_chip/AvatarSafetyPoseController.h"
#include <cassert>
int main(){ dnxr::AvatarRigCommandFrame f; f.joints.push_back({dnxr::BodyJoint::RightHand,{1,0,1,0},1,0,0}); dnxr::AvatarSafetyPoseController c; auto o=c.apply(f,true); assert(o.footLock01==1.0f); assert(o.handReachConfidence01==1.0f); assert(o.joints[0].damping01==1.0f); }
