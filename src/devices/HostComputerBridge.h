#pragma once

#include <vector>

namespace dnxr {

class HostComputerBridge {
public:
    /// Sends signal data from edge devices to host-side processing.
    std::vector<float> uplink(const std::vector<float>& signal) const;
    /// Returns host-generated command data to edge devices.
    std::vector<float> downlink(const std::vector<float>& command) const;
};

} // namespace dnxr
