#pragma once

#include "../config/RuntimeConfig.h"
#include "../hardware/NeuralChipHAL.h"
#include "../hardware/INeuralChipTransport.h"
#include "RuntimeDeploymentMode.h"

#include <cstdint>
#include <memory>
#include <string>
#include <vector>

namespace dnxr {

using INeuralTransport = INeuralChipTransport;

class INeuralPacketProtector {
public:
    virtual ~INeuralPacketProtector() = default;
    virtual std::vector<std::uint8_t> protect(const std::vector<float>& neuralPayload,
                                              const std::string& keyMaterial) const = 0;
    virtual std::vector<float> unprotect(const std::vector<std::uint8_t>& protectedPayload,
                                         const std::string& keyMaterial) const = 0;
    virtual bool cryptographicallySecure() const noexcept = 0;
    virtual bool simulationOnly() const noexcept = 0;
};

struct NeuralSignalReadResult {
    NeuralChipFrame frame;
    std::string provenance = "unavailable";
    bool valid = false;
    bool duplicateSequence = false;
    bool staleTimestamp = false;
    std::string failureReason = "not_read";
};

class INeuralSignalSource {
public:
    virtual ~INeuralSignalSource() = default;
    virtual NeuralSignalReadResult readCanonicalFrame(double cycleTimestampSeconds) = 0;
};

enum class BiometricValidity {
    Valid,
    Unavailable,
    Invalid
};

struct BiometricSample {
    std::vector<float> channels;
    double timestampSeconds = 0.0;
    float quality = 0.0f;
    std::string provenance = "unavailable";
    BiometricValidity validity = BiometricValidity::Unavailable;

    bool usable() const noexcept {
        return validity == BiometricValidity::Valid && quality > 0.0f && !channels.empty();
    }
};

class IBiometricSource {
public:
    virtual ~IBiometricSource() = default;
    virtual BiometricSample readBiometricSample(double timestampSeconds) = 0;
};

struct RuntimeHardwareEvidence {
    bool deviceDiscovered = false;
    bool hardwareIdentityVerified = false;
    bool firmwareVerified = false;
    bool deviceCertificateVerified = false;
    bool capabilitiesNegotiated = false;
    bool readChannelVerified = false;
    bool feedbackChannelVerified = false;
    bool hardwareCutoffVerified = false;
    bool clocksSynchronized = false;
    bool authenticatedSessionCreated = false;
};

struct RuntimeComposition {
    RuntimeDeploymentMode deploymentMode = RuntimeDeploymentMode::SimulationOnly;
    RuntimeHardwareEvidence evidence;
    std::unique_ptr<INeuralChipHAL> neuralChipHal;
    std::unique_ptr<INeuralTransport> neuralTransport;
    std::unique_ptr<INeuralPacketProtector> packetProtector;
    std::unique_ptr<INeuralSignalSource> neuralSignalSource;
    std::unique_ptr<IBiometricSource> biometricSource;
};

class RuntimeCompositionRoot {
public:
    RuntimeComposition compose(const RuntimeConfig& config) const;
};

} // namespace dnxr
