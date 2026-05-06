#include "StabilityMonitor.h"

namespace dnxr {

bool StabilityMonitor::checkCardioRespiratoryWindow(const std::vector<float>& telemetry) const {
    // This function validates heart and respiration ranges against conservative safety bounds.
    if (telemetry.size() < 2) {
        return false;
    }

    const float bpm = telemetry[0];
    const float rpm = telemetry[1];
    return bpm > 45.0F && bpm < 130.0F && rpm > 8.0F && rpm < 28.0F;
}

} // namespace dnxr
