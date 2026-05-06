#include "NanoNodeInterface.h"

#include <stdexcept>
#include <utility>

#include "../utils/Logger.h"

namespace dnxr {

NanoNodeInterface::NanoNodeInterface(NanoNodeRuntimeConfig config) : config_(std::move(config)) {
    // This function stores immutable runtime credentials required for real nano-device sessions.
    validateRuntimeConfig();
}

std::vector<float> NanoNodeInterface::readNeuralNodeSignals() const {
    // This function enforces real hardware integration by rejecting simulated reads.
    validateRuntimeConfig();
    throw std::runtime_error(
        "Real nano-node acquisition backend is not wired yet. Connect vendor SDK stream before calling readNeuralNodeSignals().");
}

void NanoNodeInterface::writeNeuralStimulus(const std::vector<float>& vec) const {
    // This function enforces real hardware integration by rejecting simulated writes.
    validateRuntimeConfig();
    Logger::info("Dispatching real nano stimulus payload size=" + std::to_string(vec.size()) +
                 " toward endpoint=" + config_.endpoint);
    throw std::runtime_error(
        "Real nano-node stimulation backend is not wired yet. Connect vendor SDK writer before calling writeNeuralStimulus().");
}

void NanoNodeInterface::validateRuntimeConfig() const {
    // This function guarantees that real sessions always contain endpoint, token, and scale metadata.
    if (config_.endpoint.empty() || config_.authToken.empty() || config_.expectedNodeCount == 0U) {
        throw std::invalid_argument(
            "NanoNodeRuntimeConfig is invalid: endpoint, authToken, and expectedNodeCount are mandatory for real usage.");
    }
}

} // namespace dnxr
