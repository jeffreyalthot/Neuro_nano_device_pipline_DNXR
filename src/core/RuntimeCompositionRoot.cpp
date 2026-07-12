#include "RuntimeCompositionRoot.h"
#include "../network/OpenSSLNeuralPacketProtector.h"

#if DNXR_SIMULATION_ONLY
#include "../network/SimulationCryptoTransport.h"
#endif

#include <stdexcept>

namespace dnxr {
namespace {
class NullNeuralTransport final : public INeuralTransport {};

#if DNXR_SIMULATION_ONLY
class SimulationPacketProtectorAdapter final : public INeuralPacketProtector {
public:
    std::vector<std::uint8_t> protect(const std::vector<float>& neuralPayload,
                                      const std::string& keyMaterial) const override {
        return crypto_.encrypt(neuralPayload, keyMaterial);
    }
    std::vector<float> unprotect(const std::vector<std::uint8_t>& protectedPayload,
                                 const std::string& keyMaterial) const override {
        return crypto_.decrypt(protectedPayload, keyMaterial);
    }
    bool cryptographicallySecure() const noexcept override { return SimulationCryptoTransport::kCryptographicallySecure; }
    bool simulationOnly() const noexcept override { return SimulationCryptoTransport::kSimulationOnly; }
private:
    SimulationCryptoTransport crypto_;
};
#endif

std::unique_ptr<INeuralPacketProtector> makePacketProtector(bool simulationMode) {
#if DNXR_SIMULATION_ONLY
    if (simulationMode) {
        return std::make_unique<SimulationPacketProtectorAdapter>();
    }
#else
    if (simulationMode) {
        throw std::runtime_error("SIMULATION_COMPONENTS_NOT_COMPILED_IN_REAL_BUILD");
    }
#endif
    return std::make_unique<OpenSSLNeuralPacketProtector>();
}

class NullNeuralSignalSource final : public INeuralSignalSource {};
class NullBiometricSource final : public IBiometricSource {};
}

RuntimeComposition RuntimeCompositionRoot::compose(const RuntimeConfig& config) const {
    RuntimeComposition composition;
    composition.neuralTransport = std::make_unique<NullNeuralTransport>();
    composition.packetProtector = makePacketProtector(config.simulationMode);
    composition.neuralSignalSource = std::make_unique<NullNeuralSignalSource>();
    composition.biometricSource = std::make_unique<NullBiometricSource>();

    if (config.simulationMode) {
#if DNXR_SIMULATION_ONLY
        composition.deploymentMode = RuntimeDeploymentMode::SimulationOnly;
        composition.neuralChipHal = std::make_unique<SimulatedNeuralChipHAL>(64);
        composition.neuralChipHal->connect("runtime_simulated_neural_chip");
        return composition;
#else
        throw std::runtime_error("SIMULATION_HAL_NOT_COMPILED_IN_REAL_BUILD");
#endif
    }

    composition.deploymentMode = RuntimeDeploymentMode::HardwareBenchNoHuman;
    auto hal = std::make_unique<RealNeuralChipHAL>();
    hal->connect("runtime_real_neural_chip");
    composition.evidence.deviceDiscovered = true;
    composition.evidence.hardwareIdentityVerified = hal->deviceIdentity().signedIdentity;
    composition.evidence.firmwareVerified = !hal->deviceIdentity().firmwareHash.empty();
    composition.evidence.deviceCertificateVerified = !hal->deviceIdentity().certificateHash.empty();
    composition.evidence.capabilitiesNegotiated = hal->capabilities().electrodeCount > 0 || hal->capabilities().maxStimulusChannels > 0;
    composition.evidence.readChannelVerified = hal->status().connected;
    composition.evidence.feedbackChannelVerified = hal->closedLoopFeedbackAvailable();
    composition.evidence.hardwareCutoffVerified = hal->hardwareCutoffAvailable();
    composition.evidence.clocksSynchronized = false;
    composition.evidence.authenticatedSessionCreated = false;

    if (!composition.evidence.hardwareIdentityVerified ||
        !composition.evidence.firmwareVerified ||
        !composition.evidence.deviceCertificateVerified ||
        !composition.evidence.capabilitiesNegotiated ||
        !composition.evidence.readChannelVerified ||
        !composition.evidence.feedbackChannelVerified ||
        !composition.evidence.hardwareCutoffVerified ||
        !composition.evidence.clocksSynchronized ||
        !composition.evidence.authenticatedSessionCreated) {
        throw std::runtime_error("REAL_HARDWARE_STARTUP_EVIDENCE_INCOMPLETE");
    }

    composition.neuralChipHal = std::move(hal);
    return composition;
}

} // namespace dnxr
