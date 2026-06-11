#pragma once

#include <cstddef>
#include <cstdint>
#include <string>

namespace dnxr {

struct NeuralChipReplayPlaybackReport {
    bool valid = false;
    const char* reason = "";
    std::size_t framesRead = 0;
};

class NeuralChipReplayPlayer {
public:
    NeuralChipReplayPlaybackReport validateJsonl(const std::string& path) const;
};

} // namespace dnxr
