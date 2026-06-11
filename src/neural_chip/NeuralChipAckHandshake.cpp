#include "neural_chip/NeuralChipAckHandshake.h"

#include <algorithm>

namespace dnxr {

NeuralChipAckHandshake::NeuralChipAckHandshake(double timeoutMs) : timeoutMs_(timeoutMs) {}
float NeuralChipAckHandshake::clamp01(float value) { return std::max(0.0f, std::min(1.0f, value)); }
void NeuralChipAckHandshake::expect(std::uint64_t frameId, double timestampMs) {
    state_.expectedFrameId = frameId;
    state_.ackReceived = false;
    state_.timeout = false;
    state_.ackQuality01 = 0.0f;
    state_.expectedSinceMs = timestampMs;
}
void NeuralChipAckHandshake::receiveAck(std::uint64_t frameId, float quality01) {
    if (frameId != state_.expectedFrameId) return;
    state_.ackReceived = true;
    state_.timeout = false;
    state_.ackQuality01 = clamp01(quality01);
}
bool NeuralChipAckHandshake::shouldHold(double nowMs) const {
    if (state_.ackReceived) return false;
    return (nowMs - state_.expectedSinceMs) >= timeoutMs_;
}
const NeuralChipAckState& NeuralChipAckHandshake::state() const { return state_; }

} // namespace dnxr
