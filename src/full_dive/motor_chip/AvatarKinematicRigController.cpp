#include "full_dive/motor_chip/AvatarKinematicRigController.h"
#include "full_dive/motor_chip/AvatarInverseKinematicsSolver.h"

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
    const AvatarGaitState gaitState = gait_.update(intent);
    const AvatarBalanceState balanceState = balance_.stabilize(intent, gaitState);
    const float confidence = clamp01(intent.confidence01) * (1.0f - clamp01(intent.stopIntent01));
    const float gait = gaitState.speed01;
    out.centerOfMassStability01 = balanceState.centerOfMassStability01;
    out.footLock01 = gaitState.footLock01;
    out.handReachConfidence01 = clamp01(std::max(intent.rightArmReach01, intent.leftArmReach01) * confidence);

    const float torso = intent.postureBalance01;
    const float head = std::max(intent.lookPitch01, intent.turnYaw01);
    const float rightArm = intent.rightArmReach01;
    const float leftArm = intent.leftArmReach01;
    const float rightGrip = intent.rightHandGrip01;
    const float leftGrip = intent.leftHandGrip01;
    const float lowerBody = std::max(gait, intent.postureBalance01);

    out.joints.push_back(command(BodyJoint::Root, gait, confidence));
    out.joints.push_back(command(BodyJoint::Pelvis, lowerBody, confidence));
    out.joints.push_back(command(BodyJoint::Spine, torso, confidence));
    out.joints.push_back(command(BodyJoint::Chest, torso, confidence));
    out.joints.push_back(command(BodyJoint::Neck, head, confidence));
    out.joints.push_back(command(BodyJoint::Head, head, confidence));
    out.joints.push_back(command(BodyJoint::Jaw, std::max(rightGrip, leftGrip) * 0.15f, confidence));
    out.joints.push_back(command(BodyJoint::RightEye, intent.turnYaw01, confidence));
    out.joints.push_back(command(BodyJoint::LeftEye, intent.turnYaw01, confidence));
    out.joints.push_back(command(BodyJoint::RightClavicle, rightArm, confidence));
    out.joints.push_back(command(BodyJoint::RightShoulder, intent.rightArmReach01, confidence));
    out.joints.push_back(command(BodyJoint::RightElbow, intent.rightArmReach01, confidence));
    out.joints.push_back(command(BodyJoint::RightWrist, rightGrip, confidence));
    out.joints.push_back(command(BodyJoint::RightHand, intent.rightHandGrip01, confidence));
    out.joints.push_back(command(BodyJoint::RightThumb, rightGrip, confidence));
    out.joints.push_back(command(BodyJoint::RightIndex, rightGrip, confidence));
    out.joints.push_back(command(BodyJoint::RightMiddle, rightGrip, confidence));
    out.joints.push_back(command(BodyJoint::RightRing, rightGrip, confidence));
    out.joints.push_back(command(BodyJoint::RightLittle, rightGrip, confidence));
    out.joints.push_back(command(BodyJoint::LeftClavicle, leftArm, confidence));
    out.joints.push_back(command(BodyJoint::LeftShoulder, intent.leftArmReach01, confidence));
    out.joints.push_back(command(BodyJoint::LeftElbow, intent.leftArmReach01, confidence));
    out.joints.push_back(command(BodyJoint::LeftWrist, leftGrip, confidence));
    out.joints.push_back(command(BodyJoint::LeftHand, intent.leftHandGrip01, confidence));
    out.joints.push_back(command(BodyJoint::LeftThumb, leftGrip, confidence));
    out.joints.push_back(command(BodyJoint::LeftIndex, leftGrip, confidence));
    out.joints.push_back(command(BodyJoint::LeftMiddle, leftGrip, confidence));
    out.joints.push_back(command(BodyJoint::LeftRing, leftGrip, confidence));
    out.joints.push_back(command(BodyJoint::LeftLittle, leftGrip, confidence));
    out.joints.push_back(command(BodyJoint::RightHip, gait, confidence));
    out.joints.push_back(command(BodyJoint::RightKnee, gait, confidence));
    out.joints.push_back(command(BodyJoint::RightAnkle, lowerBody, confidence));
    out.joints.push_back(command(BodyJoint::RightFoot, gait, confidence));
    out.joints.push_back(command(BodyJoint::RightToe, gait, confidence));
    out.joints.push_back(command(BodyJoint::LeftHip, gait, confidence));
    out.joints.push_back(command(BodyJoint::LeftKnee, gait, confidence));
    out.joints.push_back(command(BodyJoint::LeftAnkle, lowerBody, confidence));
    out.joints.push_back(command(BodyJoint::LeftFoot, gait, confidence));
    out.joints.push_back(command(BodyJoint::LeftToe, gait, confidence));
    AvatarInverseKinematicsSolver{}.solve(intent, balanceState, out);
    return out;
}

AvatarRigCommandFrame AvatarKinematicRigController::applyClosedLoop(
    const NeuralMotorIntentVector& intent,
    const ChipReadFrame& feedback) const {
    NeuralMotorIntentVector closedLoopIntent = intent;
    const float feedbackQuality = clamp01(feedback.signalQuality01);
    closedLoopIntent.moveForward01 = clamp01(
        intent.moveForward01 * 0.70f +
        feedback.motorIntent.moveForward01 * feedbackQuality * 0.30f);
    closedLoopIntent.gaitIntent01 = clamp01(
        intent.gaitIntent01 * 0.70f +
        feedback.motorIntent.gaitIntent01 * feedbackQuality * 0.30f);
    closedLoopIntent.postureBalance01 = clamp01(
        std::max(intent.postureBalance01,
                 feedback.motorIntent.postureBalance01 * feedbackQuality));
    closedLoopIntent.confidence01 = clamp01(
        intent.confidence01 * 0.65f + feedback.motorIntent.confidence01 *
                                         feedbackQuality * 0.35f);
    closedLoopIntent.stopIntent01 = std::max(intent.stopIntent01,
                                             feedback.motorIntent.stopIntent01);
    return apply(closedLoopIntent);
}

} // namespace dnxr
