#include "full_dive/motor_chip/AvatarSafetyPoseController.h"

namespace dnxr {

AvatarRigCommandFrame AvatarSafetyPoseController::apply(const AvatarRigCommandFrame& frame, bool keepHandsVisible) const {
    AvatarRigCommandFrame out = frame;
    out.centerOfMassStability01 = 1.0f;
    out.footLock01 = 1.0f;
    out.handReachConfidence01 = keepHandsVisible ? 1.0f : 0.0f;
    for (auto& joint : out.joints) {
        joint.targetRotation = {1.0f, 0.0f, 0.0f, 0.0f};
        joint.stiffness01 = 0.15f;
        joint.damping01 = 1.0f;
        joint.confidence01 = 1.0f;
        if (keepHandsVisible && (joint.joint == BodyJoint::RightHand || joint.joint == BodyJoint::LeftHand ||
                                 joint.joint == BodyJoint::RightShoulder || joint.joint == BodyJoint::LeftShoulder)) {
            joint.stiffness01 = 0.35f;
        }
    }
    return out;
}

} // namespace dnxr
