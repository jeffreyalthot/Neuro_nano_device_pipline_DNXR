#include "../src/core/CanonicalRuntimeHost.h"
#include "../src/core/SessionBootstrapValidator.h"

#include <cassert>
#include <type_traits>
#include "../src/evidence/CryptoSignatureVerifier.h"

namespace {

dnxr::RuntimeHardwareEvidence completeEvidence() {
    dnxr::RuntimeHardwareEvidence evidence{};
    evidence.deviceDiscovered = true;
    evidence.hardwareIdentityVerified = true;
    evidence.firmwareVerified = true;
    evidence.deviceCertificateVerified = true;
    evidence.capabilitiesNegotiated = true;
    evidence.readChannelVerified = true;
    evidence.feedbackChannelVerified = true;
    evidence.hardwareCutoffVerified = true;
    evidence.clocksSynchronized = true;
    evidence.authenticatedSessionCreated = true;
    return evidence;
}


dnxr::SignedRuntimeArtifactConfig signedArtifact(const std::string& id) {
    dnxr::SignedRuntimeArtifactConfig a;
    a.artifactId = id;
    a.issuer = "vendor";
    a.subjectId = "patient-nonhuman";
    a.deviceId = "device-1";
    a.schema = "dnxr.runtime-artifact.v1";
    a.compatibility = "fw=1;protocol=1;schema=1";
    a.payloadDigest = dnxr::CryptoSignatureVerifier::digestHex(id + "payload");
    a.keyId = "key";
    a.trustAnchor = "trust";
    a.validFromUnixSeconds = 1;
    a.validUntilUnixSeconds = 2000;
    a.auditId = id + "-audit";
    dnxr::SignedEvidenceDocument d{a.artifactId, a.payloadDigest, "", a.issuer, "runtime-artifact-issuer", a.keyId, a.validFromUnixSeconds, a.validUntilUnixSeconds};
    a.signature = dnxr::CryptoSignatureVerifier::digestHex(dnxr::CryptoSignatureVerifier::canonicalPayload(d) + "|" + a.trustAnchor);
    return a;
}

void attachArtifacts(dnxr::RuntimeConfig& config) {
    config.runtimeArtifacts.patientId = "patient-nonhuman";
    config.runtimeArtifacts.deviceId = "device-1";
    config.runtimeArtifacts.firmwareVersion = "1";
    config.runtimeArtifacts.protocolVersion = "1";
    config.runtimeArtifacts.calibration = signedArtifact("calibration");
    config.runtimeArtifacts.patientMap = signedArtifact("patient_map");
    config.runtimeArtifacts.topology = signedArtifact("topology");
    config.runtimeArtifacts.capability = signedArtifact("capability");
}

} // namespace

static_assert(!std::is_assignable<dnxr::SessionBootstrapSnapshot&, dnxr::SessionBootstrapSnapshot>::value,
              "session bootstrap snapshot must be immutable after validation");
static_assert(!std::is_copy_constructible<dnxr::CanonicalRuntimeHost>::value,
              "canonical runtime host must be single-owner and non-copyable");

int main() {
    dnxr::RuntimeConfig config{};
    config.profile = "simulation_safe";
    config.runtimeMode = "simulation_mode";
    config.simulationMode = true;
    config.virtualWorldIngress.endpoint = "structured://world";
    config.neuralChipTransport.transport = "simulation_hal";

    const dnxr::SessionBootstrapValidator validator;
    const auto ok = validator.validate(config);
    assert(ok.ok());
    assert(ok.snapshot().profileKind == dnxr::RuntimeProfileKind::SimulationTestRunner);
    assert(ok.snapshot().deploymentMode == dnxr::RuntimeDeploymentMode::SimulationOnly);
    assert(dnxr::SessionBootstrapValidator::uniqueOwnershipSatisfied(ok.snapshot().instanceCounters));
    assert(ok.snapshot().destructionOrder.size() == 6U);
    assert(ok.snapshot().destructionOrder[0] == "stop_dispatch");
    assert(ok.snapshot().destructionOrder[5] == "destroy_transport");

    dnxr::RuntimeComposition composition{};
    composition.deploymentMode = ok.snapshot().deploymentMode;
    auto host = dnxr::CanonicalRuntimeHost::create(config, std::move(composition), ok.snapshot());
    assert(host.has_value());
    assert(host->uniqueOwnershipSatisfied());
    assert(!host->canReconfigureBackend(dnxr::RuntimeDeploymentMode::HardwareBenchNoHuman));
    assert(host->canReconfigureBackend(dnxr::RuntimeDeploymentMode::SimulationOnly));

    dnxr::RuntimeInstanceCounters duplicateOwners{};
    duplicateOwners.sessionOwners = 2;
    duplicateOwners.monotonicClocks = 1;
    duplicateOwners.committers = 1;
    duplicateOwners.ackHandshakes = 1;
    duplicateOwners.emergencyLatches = 1;
    duplicateOwners.stateMachines = 1;
    assert(!dnxr::SessionBootstrapValidator::uniqueOwnershipSatisfied(duplicateOwners));

    dnxr::RuntimeConfig invalid = config;
    invalid.profile.clear();
    const auto invalidResult = validator.validate(invalid);
    assert(!invalidResult.ok());
    assert(invalidResult.error().reason == "profile_missing");

    dnxr::RuntimeConfig real = config;
    real.profile = "real_minimal";
    real.runtimeMode = "real_mode";
    real.simulationMode = false;
    const auto rejectedReal = validator.validate(real);
    assert(!rejectedReal.ok());
    assert(rejectedReal.error().reason == "artifact_bundle_identity_incomplete");

    attachArtifacts(real);
    const auto missingHardwareReal = validator.validate(real);
    assert(!missingHardwareReal.ok());
    assert(missingHardwareReal.error().reason == "real_backend_attestation_incomplete");

    const auto acceptedReal = validator.validate(real, completeEvidence());
    assert(acceptedReal.ok());
    assert(acceptedReal.snapshot().profileKind == dnxr::RuntimeProfileKind::RealMinimal);
    assert(acceptedReal.snapshot().backendAttested);
    assert(acceptedReal.snapshot().secretsResolved);

    return 0;
}
