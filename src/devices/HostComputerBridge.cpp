#include "HostComputerBridge.h"

#include <stdexcept>
#include <utility>

#include "../utils/Logger.h"

namespace dnxr {

HostComputerBridge::HostComputerBridge(HostBridgeRuntimeConfig config) : config_(std::move(config)) {
    // This function stores immutable runtime credentials for real host-bridge networking.
    validateRuntimeConfig();
}

std::vector<float> HostComputerBridge::uplink(const std::vector<float>& signal) const {
    // This function enforces real host uplink networking by rejecting simulation-time transformations.
    validateRuntimeConfig();
    Logger::info("Forwarding real uplink payload size=" + std::to_string(signal.size()) +
                 " to host endpoint=" + config_.upstreamEndpoint);
    throw std::runtime_error(
        "Real host uplink backend is not wired yet. Connect production transport before calling uplink().");
}

std::vector<float> HostComputerBridge::downlink(const std::vector<float>& command) const {
    // This function enforces real host downlink networking by rejecting synthetic command shaping.
    validateRuntimeConfig();
    Logger::info("Forwarding real downlink payload size=" + std::to_string(command.size()) +
                 " to edge endpoint=" + config_.downstreamEndpoint);
    throw std::runtime_error(
        "Real host downlink backend is not wired yet. Connect production transport before calling downlink().");
}

void HostComputerBridge::validateRuntimeConfig() const {
    // This function guarantees endpoint and token presence for authenticated production transport.
    if (config_.upstreamEndpoint.empty() || config_.downstreamEndpoint.empty() || config_.authToken.empty()) {
        throw std::invalid_argument(
            "HostBridgeRuntimeConfig is invalid: upstreamEndpoint, downstreamEndpoint, and authToken are mandatory for real usage.");
    }
}

} // namespace dnxr
