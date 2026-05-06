#pragma once

#include <cstdint>

namespace dnxr {

class TimeSync {
public:
    /// Returns a monotonic timestamp in milliseconds for sequencing events.
    static std::int64_t nowMs();
};

} // namespace dnxr
