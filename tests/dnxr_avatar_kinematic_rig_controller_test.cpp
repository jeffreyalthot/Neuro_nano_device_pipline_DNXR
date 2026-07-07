#include "full_dive/motor_chip/AvatarKinematicRigController.h"
#include <cassert>

int main() {
    dnxr::NeuralMotorIntentVector intent;
    intent.moveForward01 = 0.6f;
    intent.gaitIntent01 = 0.3f;
    intent.rightArmReach01 = 0.7f;
    intent.rightHandGrip01 = 0.6f;
    intent.confidence01 = 0.8f;
    intent.stopIntent01 = 0.0f;

    dnxr::ChipReadFrame feedback;
    feedback.signalQuality01 = 0.9f;
    feedback.motorIntent.moveForward01 = 0.8f;
    feedback.motorIntent.gaitIntent01 = 0.7f;
    feedback.motorIntent.postureBalance01 = 0.9f;
    feedback.motorIntent.confidence01 = 0.9f;
    feedback.motorIntent.stopIntent01 = 0.0f;

    const auto rig = dnxr::AvatarKinematicRigController{}.applyClosedLoop(intent, feedback);
    assert(rig.joints.size() >= 38);
    assert(rig.handReachConfidence01 > 0.0f);
    assert(rig.footLock01 < 1.0f);
    return 0;
}
