#pragma once

#include "neural_chip/NeuralChipFrame.h"
#include "neural_chip/SafeChipFrameHoldBuffer.h"

namespace dnxr {

enum class NeuralChipHoldFallback {
    Decompression,
    Neutral,
    MinimalHard,
    BusSafetyOnly
};

class NeuralChipHoldCommitPolicy {
public:
    static NeuralRealityAnchorVector makeSafeHoldRealityAnchor();
    FullDiveChipFrame makeDecompressionHoldFrame(const FullDiveChipFrame& rejectedFrame, const SafeChipFrameHoldBuffer& holdBuffer, float ramp01) const;
    FullDiveChipFrame makeNeutralHoldFrame(const FullDiveChipFrame& rejectedFrame, const SafeChipFrameHoldBuffer& holdBuffer) const;
    FullDiveChipFrame makeMinimalHardHoldFrame(std::uint64_t frameId, double timestampMs) const;
    const char* fallbackName(NeuralChipHoldFallback fallback) const;

private:
    static void forceHoldSafety(FullDiveChipFrame& frame);
};

} // namespace dnxr
