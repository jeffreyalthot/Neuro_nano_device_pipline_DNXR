#include "full_dive/motor_chip/AvatarKinematicRigController.h"

#include <algorithm>

namespace dnxr {

float AvatarKinematicRigController::clamp01(float value) {
    return std::max(0.0f, std::min(1.0f, value));
}

AvatarJointCommand AvatarKinematicRigController::command(BodyJoint joint, float activation01, float confidence01) {
    AvatarJointCommand out;
    out.joint = joint;
    out.targetRotation = {1.0f, 0.0f, clamp01(activation01), 0.0f};
    out.stiffness01 = clamp01(activation01);
    out.damping01 = 1.0f - out.stiffness01 * 0.5f;
    out.confidence01 = clamp01(confidence01);
    return out;
}

AvatarRigCommandFrame AvatarKinematicRigController::apply(const NeuralMotorIntentVector& intent) const {
    AvatarRigCommandFrame out;
    const float confidence = clamp01(intent.confidence01) * (1.0f - clamp01(intent.stopIntent01));
    const float gait = clamp01(intent.gaitIntent01 + intent.moveForward01 + intent.moveBackward01);
    out.centerOfMassStability01 = clamp01(intent.postureBalance01);
    out.footLock01 = intent.stopIntent01 > 0.5f ? 1.0f : 1.0f - clamp01(gait);
    out.handReachConfidence01 = clamp01(std::max(intent.rightArmReach01, intent.leftArmReach01) * confidence);

    out.joints.push_back(command(BodyJoint::Root, gait, confidence));
    out.joints.push_back(command(BodyJoint::Spine, intent.postureBalance01, confidence));
    out.joints.push_back(command(BodyJoint::Head, intent.lookPitch01, confidence));
    out.joints.push_back(command(BodyJoint::RightShoulder, intent.rightArmReach01, confidence));
    out.joints.push_back(command(BodyJoint::RightElbow, intent.rightArmReach01, confidence));
    out.joints.push_back(command(BodyJoint::RightHand, intent.rightHandGrip01, confidence));
    out.joints.push_back(command(BodyJoint::LeftShoulder, intent.leftArmReach01, confidence));
    out.joints.push_back(command(BodyJoint::LeftElbow, intent.leftArmReach01, confidence));
    out.joints.push_back(command(BodyJoint::LeftHand, intent.leftHandGrip01, confidence));
    out.joints.push_back(command(BodyJoint::RightHip, gait, confidence));
    out.joints.push_back(command(BodyJoint::RightKnee, gait, confidence));
    out.joints.push_back(command(BodyJoint::RightFoot, gait, confidence));
    out.joints.push_back(command(BodyJoint::LeftHip, gait, confidence));
    out.joints.push_back(command(BodyJoint::LeftKnee, gait, confidence));
    out.joints.push_back(command(BodyJoint::LeftFoot, gait, confidence));
    return out;
}

} // namespace dnxr
