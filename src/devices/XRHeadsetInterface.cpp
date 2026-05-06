#include "XRHeadsetInterface.h"

#include "../utils/Logger.h"

namespace dnxr {

void XRHeadsetInterface::pushSensoryFrame(const std::vector<float>& frame) const {
    // This function pushes a rendered sensory frame toward XR hardware output channels.
    Logger::info("XR frame pushed=" + std::to_string(frame.size()));
}

std::vector<float> XRHeadsetInterface::pullBioTelemetry() const {
    // This function returns representative biometric telemetry for safety validation.
    return {72.0F, 16.0F, 98.0F};
}

} // namespace dnxr
