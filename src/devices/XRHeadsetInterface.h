#pragma once

#include <string>
#include <vector>

namespace dnxr {

struct XRHeadsetRuntimeConfig {
    std::string endpoint;
    std::string deviceId;
    std::string authToken;
};

class XRHeadsetInterface {
public:
    /// Builds a real XR-headset interface with authenticated transport configuration.
    explicit XRHeadsetInterface(XRHeadsetRuntimeConfig config);

    /// Pushes a computed sensory frame to the XR device.
    void pushSensoryFrame(const std::vector<float>& frame) const;

    /// Pulls biometric telemetry from the XR wearable sensors.
    std::vector<float> pullBioTelemetry() const;

private:
    /// Verifies that runtime configuration is complete before I/O with the real headset.
    void validateRuntimeConfig() const;

    XRHeadsetRuntimeConfig config_;
};

} // namespace dnxr
