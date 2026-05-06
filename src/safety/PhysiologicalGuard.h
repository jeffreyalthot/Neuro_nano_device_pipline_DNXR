#pragma once

#include "EmergencyShutdown.h"
#include "StabilityMonitor.h"
#include <vector>

namespace dnxr {

class PhysiologicalGuard {
public:
    /// Enforces physiological safety before a runtime cycle proceeds.
    bool enforce(const std::vector<float>& telemetry) const;

private:
    StabilityMonitor monitor_;
    EmergencyShutdown killSwitch_;
};

} // namespace dnxr
