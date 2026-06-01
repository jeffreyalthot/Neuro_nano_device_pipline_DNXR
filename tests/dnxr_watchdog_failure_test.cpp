#include "../src/safety/IndependentWatchdog.h"

#include <cassert>
#include <chrono>
#include <iostream>

int main() {
    using clock = std::chrono::steady_clock;
    const auto now = clock::now();

    dnxr::IndependentWatchdog watchdog(10.0, 25.0);
    watchdog.heartbeat("runtime", now);
    watchdog.heartbeat("renderer", now);

    const auto nominal = watchdog.assess(11.0, true, 0.10f, 0.10f, now + std::chrono::milliseconds(1));
    assert(nominal.healthy);
    assert(!nominal.requiresEmergencyStop);

    const auto stale = watchdog.assess(11.0, true, 0.10f, 0.10f, now + std::chrono::milliseconds(20));
    assert(!stale.healthy);
    assert(!stale.actions.empty());

    const auto invalidSignal = watchdog.assess(11.0, false, 0.10f, 0.10f, now + std::chrono::milliseconds(1));
    assert(!invalidSignal.healthy);
    assert(invalidSignal.requiresEmergencyStop);

    const auto dangerousBio = watchdog.assess(11.0, true, 0.95f, 0.10f, now + std::chrono::milliseconds(1));
    assert(!dangerousBio.healthy);
    assert(dangerousBio.requiresEmergencyStop);

    std::cout << "Independent watchdog failure paths OK\n";
    return 0;
}
