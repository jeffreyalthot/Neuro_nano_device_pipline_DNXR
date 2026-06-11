#include "neural_chip/HoldFrameSequencer.h"

#include <algorithm>
#include <cmath>

namespace dnxr {

void HoldFrameSequencer::observeCommitted(std::uint64_t frameId, double timestampMs) {
    if (!std::isfinite(timestampMs)) return;
    if (!hasLast_ || frameId > lastFrameId_) lastFrameId_ = frameId;
    if (!hasLast_ || timestampMs > lastTimestampMs_) lastTimestampMs_ = timestampMs;
    hasLast_ = true;
}

HoldFrameToken HoldFrameSequencer::nextHold(double requestedTimestampMs, const char* holdKind) {
    HoldFrameToken token;
    token.frameId = hasLast_ ? lastFrameId_ + 1U : 1U;
    token.timestampMs = hasLast_ ? lastTimestampMs_ + 1.0 : 1.0;
    if (std::isfinite(requestedTimestampMs)) {
        if (!hasLast_) token.timestampMs = std::max(0.0, requestedTimestampMs);
        else if (requestedTimestampMs > lastTimestampMs_ && requestedTimestampMs - lastTimestampMs_ <= 100.0) token.timestampMs = requestedTimestampMs;
    }
    token.holdKind = holdKind ? holdKind : "hold";
    return token;
}

} // namespace dnxr
