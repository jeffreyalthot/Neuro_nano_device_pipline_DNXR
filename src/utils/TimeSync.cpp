#include "TimeSync.h"

#include <chrono>

namespace dnxr {

std::int64_t TimeSync::nowMs() {
    // This function returns a monotonic millisecond timestamp for stable event sequencing.
    using namespace std::chrono;
    return duration_cast<milliseconds>(steady_clock::now().time_since_epoch()).count();
}

} // namespace dnxr
