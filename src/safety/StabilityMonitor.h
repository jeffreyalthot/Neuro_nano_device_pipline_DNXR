#pragma once

#include <vector>

namespace dnxr {

class StabilityMonitor {
public:
    /// Validates cardio-respiratory telemetry against safety thresholds.
    bool checkCardioRespiratoryWindow(const std::vector<float>& telemetry) const;
};

} // namespace dnxr
