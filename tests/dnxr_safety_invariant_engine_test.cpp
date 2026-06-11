#include "neural_chip/SafetyInvariantEngine.h"
#include <cassert>
using namespace dnxr;
int main() {
    SafetyInvariantEngine engine;
    FullDiveChipFrame frame;
    NeuralChipSessionState session;
    NeuralChipAckState ack;
    ChipCapabilities caps;
    caps.canHaptic = true;
    auto ok = engine.validate(frame, session, ack, caps);
    assert(ok.accepted);
    frame.safety.rawSensoryPayloadBlocked = false;
    auto fatal = engine.validate(frame, session, ack, caps);
    assert(!fatal.accepted && fatal.triggerEmergencyLatch);
    frame.safety.rawSensoryPayloadBlocked = true;
    frame.olfactory.odorPresence01 = 0.5f;
    auto chemical = engine.validate(frame, session, ack, caps);
    assert(!chemical.accepted && chemical.requireHold);
    return 0;
}
