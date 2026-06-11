#include "neural_chip/FrameLifecycleGuard.h"
#include "neural_chip/NeuralChipAckHandshake.h"
#include "neural_chip/NeuralChipTimebase.h"
#include <cassert>
using namespace dnxr;
int main() {
    NeuralChipTimebase t;
    t.setSimulatedNowMs(100.0);
    t.advanceMs(25.0);
    assert(t.nowMs() == 125.0);
    FrameLifecycleGuard guard;
    auto policy = FrameLifecyclePolicy{};
    policy.nowMs = t.nowMs();
    guard.setPolicy(policy);
    FullDiveChipFrame oldFrame;
    oldFrame.frameId = 1;
    oldFrame.timestampMs =  -1.0;
    assert(!guard.preview(oldFrame).accepted);
    NeuralChipAckHandshake ack(50.0);
    ack.expect(7, t.nowMs());
    assert(!ack.shouldHold(t.nowMs() + 49.0));
    assert(ack.shouldHold(t.nowMs() + 50.0));
    return 0;
}
