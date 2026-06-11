#pragma once

#include "full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.h"
#include "full_dive/chip_pipeline/WorldFrameSanitizer.h"
#include "neural_chip/NeuralChipFrame.h"

#include <cstdint>

namespace dnxr {

class SanitizeFailureFrameFactory {
public:
    static FullDiveChipFrame makeSanitizedFailureProbe(std::uint64_t fallbackFrameId,
                                                       double fallbackTimestampMs,
                                                       const char* reason = "world frame rejected");
    static FullDiveChipFrame makeFailureProbe(const NeuralChipOnlyWorldFrame& rawWorld,
                                              const WorldFrameSanitizationResult& result,
                                              std::uint64_t fallbackFrameId,
                                              double fallbackTimestampMs);
};

} // namespace dnxr
