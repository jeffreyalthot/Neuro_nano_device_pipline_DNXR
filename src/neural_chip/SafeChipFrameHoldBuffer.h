#pragma once

#include "neural_chip/NeuralChipFrame.h"

#include <cstdint>

namespace dnxr {

class SafeChipFrameHoldBuffer {
public:
    void rememberAccepted(const FullDiveChipFrame& frame);
    bool hasAcceptedFrame() const;
    FullDiveChipFrame makeNeutralHoldFrame(std::uint64_t frameId, double timestampMs) const;
    FullDiveChipFrame makeDecompressionFrame(std::uint64_t frameId, double timestampMs, float ramp01) const;

private:
    static float clamp01(float value);
    static void applyHoldSafety(FullDiveChipFrame& frame);
    FullDiveChipFrame lastAccepted_{};
    bool hasAccepted_ = false;
};

} // namespace dnxr
