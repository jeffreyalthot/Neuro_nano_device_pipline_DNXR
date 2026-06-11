#pragma once

#include "neural_chip/NeuralChipFrame.h"

#include <vector>

namespace dnxr {

struct Quaternion {
    float w = 1.0f;
    float x = 0.0f;
    float y = 0.0f;
    float z = 0.0f;
};

enum class BodyJoint {
    Root,
    Spine,
    Head,
    RightShoulder,
    RightElbow,
    RightHand,
    LeftShoulder,
    LeftElbow,
    LeftHand,
    RightHip,
    RightKnee,
    RightFoot,
    LeftHip,
    LeftKnee,
    LeftFoot
};

struct AvatarJointCommand {
    BodyJoint joint = BodyJoint::Root;
    Quaternion targetRotation;
    float stiffness01 = 0.0f;
    float damping01 = 0.0f;
    float confidence01 = 0.0f;
};

struct AvatarRigCommandFrame {
    std::vector<AvatarJointCommand> joints;
    float centerOfMassStability01 = 0.0f;
    float footLock01 = 0.0f;
    float handReachConfidence01 = 0.0f;
};

class AvatarKinematicRigController {
public:
    AvatarRigCommandFrame apply(const NeuralMotorIntentVector& intent) const;

private:
    static float clamp01(float value);
    static AvatarJointCommand command(BodyJoint joint, float activation01, float confidence01);
};

} // namespace dnxr
