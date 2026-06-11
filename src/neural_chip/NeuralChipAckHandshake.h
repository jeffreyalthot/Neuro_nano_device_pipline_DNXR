#pragma once

#include <cstdint>

namespace dnxr {

struct NeuralChipAckState {
    std::uint64_t expectedFrameId = 0;
    bool hasPendingAck = false;
    bool ackReceived = true;
    bool timeout = false;
    float ackQuality01 = 1.0f;
    double expectedSinceMs = 0.0;
    std::uint64_t rejectedAckFrameId = 0;
    bool wrongFrameIdRejected = false;
};

class NeuralChipAckHandshake {
public:
    explicit NeuralChipAckHandshake(double timeoutMs = 50.0);
    void expect(std::uint64_t frameId, double timestampMs);
    void receiveAck(std::uint64_t frameId, float quality01);
    bool shouldHold(double nowMs) const;
    const NeuralChipAckState& state() const;

private:
    static float clamp01(float value);
    double timeoutMs_ = 50.0;
    NeuralChipAckState state_{};
};

} // namespace dnxr
