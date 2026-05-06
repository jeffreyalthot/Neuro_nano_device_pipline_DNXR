#pragma once

#include <vector>

namespace dnxr {

class XRHeadsetInterface {
public:
    /// Pushes a computed sensory frame to the XR device.
    void pushSensoryFrame(const std::vector<float>& frame) const;
    /// Pulls biometric telemetry from the XR wearable sensors.
    std::vector<float> pullBioTelemetry() const;
};

} // namespace dnxr
