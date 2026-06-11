#pragma once

#include "neural_chip/NeuralChipFrame.h"
#include "neural_chip/NeuralChipPipelineStateGate.h"

#include <cstdint>

namespace dnxr {

class AckHoldFrameFactory {
public:
    FullDiveChipFrame makeAckHoldFrame(std::uint64_t frameId, double timestampMs) const;
    FullDiveChipFrame makeStateHoldFrame(const PipelineStateDecision& decision, std::uint64_t frameId, double timestampMs) const;
};

} // namespace dnxr
