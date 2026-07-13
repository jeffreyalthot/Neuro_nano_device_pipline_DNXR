// TimeSync.cpp

#include "TimeSync.h"

#include <chrono>
#include <atomic>

namespace dnxr {

namespace {

std::atomic<std::int64_t> g_lastTimestamp{0};

std::int64_t monotonicNowMs() {
    using namespace std::chrono;
    return duration_cast<milliseconds>(
        steady_clock::now().time_since_epoch()
    ).count();
}

} // namespace

std::int64_t TimeSync::nowMs() {

    const std::int64_t raw = monotonicNowMs();
    std::int64_t last = g_lastTimestamp.load(std::memory_order_relaxed);
    for (;;) {
        const std::int64_t next = raw > last ? raw : last + 1;
        if (g_lastTimestamp.compare_exchange_weak(
                last, next, std::memory_order_relaxed)) {
            return next;
        }
    }
}


void TimeSync::reset() {

    g_lastTimestamp.store(0, std::memory_order_relaxed);
}

std::int64_t TimeSync::lastTimestamp() {

    return g_lastTimestamp.load(std::memory_order_relaxed);
}

} // namespace dnxr