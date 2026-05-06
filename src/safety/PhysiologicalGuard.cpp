#include "PhysiologicalGuard.h"

namespace dnxr {

bool PhysiologicalGuard::enforce(const std::vector<float>& telemetry) const {
    // This function enforces safety by validating telemetry and triggering shutdown when unsafe.
    if (!monitor_.checkCardioRespiratoryWindow(telemetry)) {
        killSwitch_.trigger();
        return false;
    }
    return true;
}

} // namespace dnxr
