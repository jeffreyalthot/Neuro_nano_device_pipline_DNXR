#pragma once

#include "neural_chip/NeuralChipReplayRecorder.h"

#include <cstdint>

namespace dnxr {

struct SessionDigest {
    std::uint64_t rollingHash = 1469598103934665603ULL;
    std::uint64_t frameCount = 0;
    std::uint64_t acceptedCount = 0;
    std::uint64_t rejectedCount = 0;
};

class NeuralChipSessionDigest {
public:
    void update(const NeuralChipReplayRecord& record);
    const SessionDigest& value() const;
    void reset();

private:
    static std::uint64_t mix(std::uint64_t hash, std::uint64_t value);
    SessionDigest digest_{};
};

} // namespace dnxr
