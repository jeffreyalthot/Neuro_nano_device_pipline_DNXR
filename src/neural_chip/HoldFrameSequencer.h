#pragma once

#include <cstdint>

namespace dnxr {

struct HoldFrameToken {
    std::uint64_t frameId = 1;
    double timestampMs = 1.0;
    const char* holdKind = "hold";
};

class HoldFrameSequencer {
public:
    void observeCommitted(std::uint64_t frameId, double timestampMs);
    HoldFrameToken nextHold(double requestedTimestampMs, const char* holdKind);

private:
    bool hasLast_ = false;
    std::uint64_t lastFrameId_ = 0;
    double lastTimestampMs_ = 0.0;
};

} // namespace dnxr
