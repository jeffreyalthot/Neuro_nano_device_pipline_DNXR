#include "RuntimeCompositionRoot.h"
#include "../network/OpenSSLNeuralPacketProtector.h"
#include "../hardware/RealNeuralChipTransport.h"
#include "../utils/TimeSync.h"

#if DNXR_SIMULATION_ONLY
#include "../network/SimulationCryptoTransport.h"
#endif

#include <stdexcept>
#include <limits>

namespace dnxr {
namespace {
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


class UnavailableWorldIngressSource final : public IWorldIngress {
public:
    VirtualWorldIngressResult readWorldFrame(const std::string&, double cycleTimestampMs) override {
        return ingress_.disconnectHold(0, cycleTimestampMs);
    }
private:
    VirtualWorldDataIngress ingress_{};
};

#if DNXR_SIMULATION_ONLY
class DeterministicSimulationWorldIngressSource final : public IWorldIngress {
public:
    DeterministicSimulationWorldIngressSource() {
        VirtualWorldIngressPolicy policy;
        policy.requireAuthentication = true;
        policy.requireMutualAuthentication = true;
        policy.expectedSessionId = "runtime-simulation-session";
        policy.maxFrameBytes = 16384;
        policy.minFrameIntervalMs = 0.0;
        ingress_ = std::make_unique<VirtualWorldDataIngress>(policy);
    }

    VirtualWorldIngressResult readWorldFrame(const std::string& expectedSessionId, double cycleTimestampMs) override {
        VirtualWorldIngressFrame raw;
        raw.frameId = ++sequence_;
        raw.timestampMs = cycleTimestampMs;
        raw.view.exposure = 0.4f;
        raw.view.contrast = 0.4f;
        raw.visualField.retinalStability = 1.0f;
        raw.audio.volume01 = 0.0f;
        raw.body.avatarOwnership01 = 1.0f;
        raw.body.handConfidence01 = 1.0f;
        raw.body.footConfidence01 = 1.0f;
        raw.realityAnchor.orientation01 = 1.0f;
        raw.realityAnchor.identityContinuity01 = 1.0f;
        raw.realityAnchor.sessionBoundary01 = 1.0f;
        raw.realityAnchor.externalWorldRecall01 = 1.0f;
        raw.scene.semanticSceneId = "deterministic_nonhuman_world_fixture";
        raw.scene.coordinateFrame = "world_right_handed_y_up";
        raw.scene.unitSystem = VirtualWorldUnitSystem::Normalized01;
        raw.assets.push_back({"fixture:grid", "0123456789abcdef0123456789abcdef", 64, true});
        auto frame = makeCanonicalVirtualWorldFrame(raw, "deterministic_simulation_world_ingress_source");
        frame.metadata.sessionId = expectedSessionId.empty() ? "runtime-simulation-session" : expectedSessionId;
        frame.metadata.sourceId = "local_ipc_fixture";
        frame.metadata.peerCertificateDigest = "nonhuman_fixture_peer_certificate";
        frame.metadata.authenticationTag = "signed-nonhuman-fixture";
        frame.metadata.hostReceiptTimestampMs = cycleTimestampMs;
        frame.metadata.deadlineTimestampMs = cycleTimestampMs + 100.0;
        frame.metadata.messageDigest = canonicalVirtualWorldDigest(frame);
        frame.bodyContract.avatarBodyProfileHash = 1;
        frame.bodyContract.patientBodyProfileHash = 1;
        return ingress_->ingestCanonical(frame, cycleTimestampMs);
    }
private:
    std::uint64_t sequence_ = 0;
    std::unique_ptr<VirtualWorldDataIngress> ingress_;
};
#endif


class HoldOnlyMotorWorldEgress final : public IMotorWorldEgress {
public:
    bool publishMotorIntent(std::uint64_t, const std::string&, const VirtualWorldMotorIngress& intent) override {
        lastIntent_ = intent;
        return intent.stopIntent01 >= 1.0f || intent.confidence01 <= 0.0f;
    }
private:
    VirtualWorldMotorIngress lastIntent_{};
};

class UnavailableBiometricSource final : public IBiometricSource {
public:
    BiometricSample readBiometricSample(double timestampSeconds) override {
        BiometricSample sample;
        sample.timestampSeconds = timestampSeconds;
        sample.quality = 0.0f;
        sample.provenance = "no_certified_biometric_source_configured";
        sample.validity = BiometricValidity::Unavailable;
        return sample;
    }
};

#if DNXR_SIMULATION_ONLY
class DeterministicSimulationBiometricSource final : public IBiometricSource {
public:
    BiometricSample readBiometricSample(double timestampSeconds) override {
        BiometricSample sample;
        sample.timestampSeconds = timestampSeconds;
        sample.channels = {72.0f, 0.5f, 0.02f};
        sample.quality = 1.0f;
        sample.provenance = "deterministic_simulation_biometric_source";
        sample.validity = BiometricValidity::Valid;
        return sample;
    }
};
#endif
}

RuntimeComposition RuntimeCompositionRoot::compose(const RuntimeConfig& config) const {
    RuntimeComposition composition;
    if (config.simulationMode) {
#if DNXR_SIMULATION_ONLY
        composition.neuralTransport = std::make_unique<SimulatedNeuralChipTransport>();
#else
        throw std::runtime_error("SIMULATION_TRANSPORT_NOT_COMPILED_IN_REAL_BUILD");
#endif
    } else {
        composition.neuralTransport = std::make_unique<ClinicalLockedNeuralChipTransport>();
    }
    composition.packetProtector = makePacketProtector(config.simulationMode);
#if DNXR_SIMULATION_ONLY
    if (config.simulationMode) {
        composition.biometricSource = std::make_unique<DeterministicSimulationBiometricSource>();
        composition.worldIngress = std::make_unique<DeterministicSimulationWorldIngressSource>();
        composition.motorWorldEgress = std::make_unique<HoldOnlyMotorWorldEgress>();
    } else
#endif
    {
        composition.biometricSource = std::make_unique<UnavailableBiometricSource>();
        composition.worldIngress = std::make_unique<UnavailableWorldIngressSource>();
        composition.motorWorldEgress = std::make_unique<HoldOnlyMotorWorldEgress>();
    }

    if (config.simulationMode) {
#if DNXR_SIMULATION_ONLY
        composition.deploymentMode = RuntimeDeploymentMode::SimulationOnly;
        composition.neuralChipHal = std::make_unique<SimulatedNeuralChipHAL>(64);
        composition.neuralChipHal->connect("runtime_simulated_neural_chip");
        BciAcquisitionValidationPolicy policy;
        policy.requireExternal = true;
        policy.requireAttested = true;
        policy.requireAuthenticated = true;
        policy.maxClockSkewNs = 1000000000000LL;
        composition.bciAcquisitionSource = std::make_unique<ExternalHalBciAcquisitionSource>(*composition.neuralChipHal, "runtime-simulation-session", true, policy);
        return composition;
#else
        throw std::runtime_error("SIMULATION_HAL_NOT_COMPILED_IN_REAL_BUILD");
#endif
    }

    composition.deploymentMode = RuntimeDeploymentMode::HardwareBenchNoHuman;
    auto hal = std::make_unique<RealNeuralChipHAL>();
    auto transport = std::make_unique<RealNeuralChipTransport>();
    const auto discovery = transport->discover("runtime_real_neural_chip");
    composition.evidence.deviceDiscovered = discovery.ok;
    const auto transportConnect = transport->connect("runtime_real_neural_chip");
    if (!transportConnect.ok) {
        throw std::runtime_error("REAL_NEURAL_TRANSPORT_UNAVAILABLE:" + transportConnect.reason);
    }
    hal->connect("runtime_real_neural_chip");
    DeviceIdentityEvidence identityEvidence;
    const DeviceIdentity identity = hal->deviceIdentity();
    identityEvidence.deviceId = identity.deviceIdHash;
    identityEvidence.certificate = identity.certificateHash;
    identityEvidence.firmwareVersion = identity.firmwareHash;
    identityEvidence.firmwareSignature = identity.signedIdentity ? identity.firmwareHash : "";
    identityEvidence.calibrationId = "runtime_calibration_unverified";
    const auto attestation = transport->attest(identityEvidence);
    composition.evidence.hardwareIdentityVerified = attestation.ok;
    composition.evidence.firmwareVerified = attestation.ok && !identity.firmwareHash.empty();
    composition.evidence.deviceCertificateVerified = attestation.ok && !identity.certificateHash.empty();
    ChipCapabilities negotiatedCapabilities;
    const auto negotiation = transport->negotiate(hal->capabilities(), negotiatedCapabilities);
    composition.evidence.capabilitiesNegotiated = negotiation.ok;
    composition.evidence.readChannelVerified = transport->read().operation.ok;
    composition.evidence.feedbackChannelVerified = hal->closedLoopFeedbackAvailable();
    composition.evidence.hardwareCutoffVerified = hal->hardwareCutoffAvailable();
    composition.evidence.clocksSynchronized = transport->synchronizeClock(TimeSync::nowMs()).ok;
    composition.evidence.authenticatedSessionCreated = transport->openAuthenticatedSession("runtime-real-session").ok;

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

    if (!hal || !hal->status().connected) {
        throw std::runtime_error("REAL_BCI_ACQUISITION_SOURCE_UNAVAILABLE");
    }
    BciAcquisitionValidationPolicy policy;
    policy.requireExternal = true;
    policy.requireAttested = true;
    policy.requireAuthenticated = true;
    policy.maxClockSkewNs = 1000000000LL;
    composition.bciAcquisitionSource = std::make_unique<ExternalHalBciAcquisitionSource>(*hal, "runtime-real-session", false, policy);
    composition.neuralTransport = std::move(transport);
    composition.neuralChipHal = std::move(hal);
    return composition;
}

} // namespace dnxr
