// TimeSync.cpp

#include "TimeSync.h"

#include <chrono>
#include <mutex>
#include <atomic>

namespace dnxr {

namespace {

std::mutex g_timeMutex;
std::atomic<std::int64_t> g_lastTimestamp{0};

std::int64_t monotonicNowMs() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(
        steady_clock::now().time_since_epoch()
    ).count();
}

} // namespace

std::int64_t TimeSync::nowMs() {

    std::lock_guard<std::mutex> lock(g_timeMutex);

    std::int64_t t = monotonicNowMs();

    // Garantit une monotonie stricte même en cas de collisions
    std::int64_t last = g_lastTimestamp.load(std::memory_order_relaxed);

    if (t <= last) {
        t = last + 1;
    }

    g_lastTimestamp.store(t, std::memory_order_relaxed);

    return t;
}

} // namespace dnxr