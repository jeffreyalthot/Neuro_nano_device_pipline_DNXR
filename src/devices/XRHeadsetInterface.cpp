#include "XRHeadsetInterface.h"

#include <stdexcept>
#include <utility>

#include "../utils/Logger.h"

namespace dnxr {

XRHeadsetInterface::XRHeadsetInterface(XRHeadsetRuntimeConfig config) : config_(std::move(config)) {
    // This function stores immutable runtime credentials required for real XR-headset sessions.
    validateRuntimeConfig();
}

void XRHeadsetInterface::pushSensoryFrame(const std::vector<float>& frame) const {
    // This function enforces real headset rendering by rejecting simulated frame delivery.
    validateRuntimeConfig();
    Logger::info("Dispatching real XR sensory frame size=" + std::to_string(frame.size()) +
                 " to headset=" + config_.deviceId);
    throw std::runtime_error(
        "Real XR rendering backend is not wired yet. Connect vendor transport before calling pushSensoryFrame().");
}

std::vector<float> XRHeadsetInterface::pullBioTelemetry() const {
    // This function enforces real telemetry acquisition by rejecting simulated biometrics.
    validateRuntimeConfig();
    throw std::runtime_error(
        "Real XR telemetry backend is not wired yet. Connect vendor sensor stream before calling pullBioTelemetry().");
}

void XRHeadsetInterface::validateRuntimeConfig() const {
    // This function guarantees endpoint, device identifier, and token are always present in real mode.
    if (config_.endpoint.empty() || config_.deviceId.empty() || config_.authToken.empty()) {
        throw std::invalid_argument(
            "XRHeadsetRuntimeConfig is invalid: endpoint, deviceId, and authToken are mandatory for real usage.");
    }
}

} // namespace dnxr
