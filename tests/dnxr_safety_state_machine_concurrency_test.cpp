#include "../src/safety/SafetyStateMachine.h"

#include <cassert>
#include <thread>
#include <vector>

int main() {
    dnxr::SafetyStateMachine machine;
    machine.bootSelfTestPassed();

    std::vector<std::thread> workers;
    for (int i = 0; i < 8; ++i) {
        workers.emplace_back([&machine, i]() {
            dnxr::SafetyDecision decision;
            decision.reason = "concurrency_probe";
            if (i == 3) {
                decision.action = dnxr::SafetyAction::EmergencyStop;
                decision.requiresEmergencyStop = true;
            } else {
                decision.action = dnxr::SafetyAction::ReduceIntensity;
            }
            machine.observeSafetyDecision(decision);
        });
    }
    for (auto& worker : workers) {
        worker.join();
    }

    assert(machine.isEmergencyLatched());
    const auto trail = machine.auditTrail();
    assert(trail.size() >= 9U);
    return 0;
}
