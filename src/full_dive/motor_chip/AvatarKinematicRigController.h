#pragma once

#include "neural_chip/NeuralChipFrame.h"
#include "full_dive/motor_chip/AvatarGaitPhaseController.h"
#include "full_dive/motor_chip/AvatarBalanceController.h"

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
    Pelvis,
    Spine,
    Chest,
    Neck,
    Head,
    Jaw,
    RightEye,
    LeftEye,
    RightClavicle,
    RightShoulder,
    RightElbow,
    RightWrist,
    RightHand,
    RightThumb,
    RightIndex,
    RightMiddle,
    RightRing,
    RightLittle,
    LeftClavicle,
    LeftShoulder,
    LeftElbow,
    LeftWrist,
    LeftHand,
    LeftThumb,
    LeftIndex,
    LeftMiddle,
    LeftRing,
    LeftLittle,
    RightHip,
    RightKnee,
    RightAnkle,
    RightFoot,
    RightToe,
    LeftHip,
    LeftKnee,
    LeftAnkle,
    LeftFoot,
    LeftToe
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
    AvatarRigCommandFrame applyClosedLoop(const NeuralMotorIntentVector& intent,
                                          const ChipReadFrame& feedback) const;

private:
    static float clamp01(float value);
    static AvatarJointCommand command(BodyJoint joint, float activation01, float confidence01);
    AvatarGaitPhaseController gait_{};
    AvatarBalanceController balance_{};
};

} // namespace dnxr
