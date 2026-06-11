#include "full_dive/safety/PerModalityIsolationBroker.h"
#include <cassert>

int main() {
    dnxr::NeuralVisualVector visual;
    visual.globalLuminance01 = 0.4f;
    dnxr::NeuralAuditoryVector auditory;
    auditory.loudness01 = 0.3f;
    dnxr::NeuralTouchVector touch;
    touch.contactPressure01 = 0.2f;
    dnxr::NeuralMotorIntentVector motor;
    motor.moveForward01 = 0.5f;

    const auto frame = dnxr::PerModalityIsolationBroker{}.assemble(
        visual, auditory, touch, {}, {}, motor, {}, {}, 7, 11.0);
    assert(frame.frameId == 7);
    assert(frame.timestampMs == 11.0);
    assert(frame.visual.globalLuminance01 == 0.4f);
    assert(frame.auditory.loudness01 == 0.3f);
    assert(frame.touch.contactPressure01 == 0.2f);
    assert(frame.motorIntent.moveForward01 == 0.5f);
    assert(frame.safety.simulationOnly);
    assert(frame.safety.rawSensoryPayloadBlocked);
    return 0;
}
