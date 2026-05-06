#pragma once

#include <string>
#include <vector>

namespace dnxr {

struct HostBridgeRuntimeConfig {
    std::string upstreamEndpoint;
    std::string downstreamEndpoint;
    std::string authToken;
};

class HostComputerBridge {
public:
    /// Builds a real host bridge with authenticated upstream and downstream links.
    explicit HostComputerBridge(HostBridgeRuntimeConfig config);

    /// Sends signal data from edge devices to host-side processing.
    std::vector<float> uplink(const std::vector<float>& signal) const;

    /// Returns host-generated command data to edge devices.
    std::vector<float> downlink(const std::vector<float>& command) const;

private:
    /// Validates endpoints and credentials before any real transport action.
    void validateRuntimeConfig() const;

    HostBridgeRuntimeConfig config_;
};

} // namespace dnxr
