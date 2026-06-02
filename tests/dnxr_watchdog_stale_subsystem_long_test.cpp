#include "../src/safety/IndependentWatchdog.h"

#include <cassert>
#include <chrono>

int main() {
    dnxr::IndependentWatchdog watchdog(50.0, 20.0);
    watchdog.requireSubsystem("renderer");
    watchdog.requireSubsystem("stimulus");

    const auto t0 = std::chrono::steady_clock::now();
    watchdog.heartbeat("renderer", t0);
    watchdog.heartbeat("stimulus", t0);
    auto healthy = watchdog.assess(8.0, true, 0.1f, 0.1f, t0 + std::chrono::milliseconds(10));
    assert(!healthy.requiresEmergencyStop);

    auto stale = watchdog.assess(8.0, true, 0.1f, 0.1f, t0 + std::chrono::milliseconds(200));
    assert(!stale.healthy);
    assert(!stale.requiresEmergencyStop);

    auto emergency = watchdog.assess(8.0, false, 0.1f, 0.1f, t0 + std::chrono::milliseconds(250));
    assert(emergency.requiresEmergencyStop);
    return 0;
}
