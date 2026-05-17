#include "../src/safety/PhysiologicalGuard.h"

#include <cassert>
#include <vector>

int main() {
    dnxr::PhysiologicalGuard guard;

    const std::vector<float> invalid = {42.0F};
    bool ok = guard.enforce(invalid);
    assert(!ok);
    assert(guard.isEmergencyActive());
    assert(guard.getEmergencyState() == dnxr::PhysiologicalGuard::EmergencyState::EmergencyLatched);
    assert(guard.getLastEmergencyReason() == "invalid_telemetry");

    const std::vector<float> nominal = {80.0F, 18.0F};
    ok = guard.enforce(nominal);
    assert(ok);
    assert(guard.getEmergencyState() == dnxr::PhysiologicalGuard::EmergencyState::Nominal);
    return 0;
}
