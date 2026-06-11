#include "full_dive/motor_chip/MotorCortexReadFrameAdapter.h"

namespace dnxr {

MotorCortexReadFrame MotorCortexReadFrameAdapter::fromChipReadFrame(const ChipReadFrame& frame) {
    MotorCortexReadFrame out;
    out.forward01 = frame.motorIntent.moveForward01;
    out.backward01 = frame.motorIntent.moveBackward01;
    out.left01 = frame.motorIntent.strafeLeft01;
    out.right01 = frame.motorIntent.strafeRight01;
    out.yaw01 = frame.motorIntent.turnYaw01;
    out.pitch01 = frame.motorIntent.lookPitch01;
    out.rightReach01 = frame.motorIntent.rightArmReach01;
    out.leftReach01 = frame.motorIntent.leftArmReach01;
    out.rightGrip01 = frame.motorIntent.rightHandGrip01;
    out.leftGrip01 = frame.motorIntent.leftHandGrip01;
    out.gait01 = frame.motorIntent.gaitIntent01;
    out.balance01 = frame.motorIntent.postureBalance01;
    out.stop01 = frame.motorIntent.stopIntent01;
    out.confidence01 = frame.signalQuality01 > 0.0f ? frame.signalQuality01 : frame.motorIntent.confidence01;
    out.tremor01 = frame.motorIntent.tremorRisk01;
    out.spasm01 = frame.motorIntent.spasmRisk01;
    out.fatigue01 = frame.motorIntent.fatigueRisk01;
    return out;
}

} // namespace dnxr
