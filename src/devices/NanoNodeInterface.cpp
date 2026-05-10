#include "NanoNodeInterface.h"

#include <stdexcept>
#include <algorithm>
#include <cmath>
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
    const auto protectedVec = applyNeuroShockProtection(vec);
    Logger::info("Dispatching real nano stimulus payload size=" + std::to_string(protectedVec.size()) +
                 " toward endpoint=" + config_.endpoint + " vendor=" + config_.vendor +
                 " protocol=" + config_.signalProtocol +
                 " accessPorts=" + config_.accessPorts);
    throw std::runtime_error(
        "Real nano-node stimulation backend is not wired yet. Connect vendor SDK writer before calling writeNeuralStimulus().");
}

std::vector<float> NanoNodeInterface::applyNeuroShockProtection(const std::vector<float>& vec) const {
    // This function simulates a future pain-absorption gate that smooths abrupt amplitude spikes.
    std::vector<float> safe(vec.size(), 0.0F);
    float previous = 0.0F;
    for (std::size_t i = 0; i < vec.size(); ++i) {
        const float clamped = std::clamp(vec[i], -config_.maxStimulusAmplitude, config_.maxStimulusAmplitude);
        const float delta = std::clamp(clamped - previous,
                                       -config_.maxStimulusDeltaPerCycle,
                                       config_.maxStimulusDeltaPerCycle);
        const float smoothed = previous + delta;
        safe[i] = smoothed * config_.painAbsorptionGain;
        previous = smoothed;
    }
    return safe;
}

const std::string& NanoNodeInterface::vendorProfile() const {
    // This function exposes the active vendor profile for routing/diagnostic decisions.
    return config_.vendor;
}

const std::string& NanoNodeInterface::accessPortProfile() const {
    // This function exposes enabled hardware access ports for routing/diagnostic decisions.
    return config_.accessPorts;
}

void NanoNodeInterface::validateRuntimeConfig() const {
    // This function guarantees that real sessions always contain endpoint, token, and scale metadata.
    if (config_.endpoint.empty() || config_.authToken.empty() || config_.vendor.empty() ||
        config_.signalProtocol.empty() || config_.accessPorts.empty() || config_.expectedNodeCount == 0U ||
        config_.maxStimulusAmplitude <= 0.0F || config_.maxStimulusDeltaPerCycle <= 0.0F ||
        config_.painAbsorptionGain <= 0.0F || config_.painAbsorptionGain > 1.0F) {
        throw std::invalid_argument(
            "NanoNodeRuntimeConfig is invalid: endpoint, authToken, vendor, signalProtocol, accessPorts, expectedNodeCount, maxStimulusAmplitude, maxStimulusDeltaPerCycle, and painAbsorptionGain (0,1] are mandatory for real usage.");
    }
}

} // namespace dnxr
