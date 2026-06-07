#include "VirtualBodyPoseProvider.h"
#include <algorithm>

namespace dnxr {

BodyPoseState VirtualBodyPoseProvider::currentAvatarPose(const RuntimePipelineFrame& frame) const {
    BodyPoseState pose;
    pose.headPose = frame.bodyCommand.headYaw;
    pose.spinePose = frame.bodyCommand.torsoLean;
    pose.armPosition = (frame.bodyCommand.leftArmLift + frame.bodyCommand.rightArmLift) * 0.5f;
    pose.handState = (frame.bodyCommand.leftHandGrip + frame.bodyCommand.rightHandGrip) * 0.5f;
    pose.legState = (frame.bodyCommand.leftLegStride + frame.bodyCommand.rightLegStride) * 0.5f;
    pose.balance = 1.0f;
    pose.virtualBodyScale = 1.0f;
    pose.virtualLimbOwnership = 1.0f;
    pose.contactPoints = std::vector<float>{std::max(frame.bodyCommand.balanceAssist, 0.01f)};
    return pose;
}

bool VirtualBodyPoseProvider::isComplete(const BodyPoseState& pose) {
    return pose.balance > 0.0f && pose.virtualBodyScale > 0.0f && pose.virtualLimbOwnership > 0.0f && !pose.contactPoints.empty();
}

} // namespace dnxr
