#include "full_dive/motor_chip/AvatarKinematicRigController.h"
#include <cassert>

int main() {
    dnxr::NeuralMotorIntentVector intent;
    intent.moveForward01 = 0.6f;
    intent.gaitIntent01 = 0.3f;
    intent.rightArmReach01 = 0.7f;
    intent.confidence01 = 0.8f;
    intent.stopIntent01 = 0.0f;

    const auto rig = dnxr::AvatarKinematicRigController{}.apply(intent);
    assert(rig.joints.size() >= 15);
    assert(rig.handReachConfidence01 > 0.0f);
    assert(rig.footLock01 < 1.0f);
    return 0;
}
