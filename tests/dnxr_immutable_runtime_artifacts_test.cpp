#include "core/SessionBootstrapValidator.h"
#include "evidence/CryptoSignatureVerifier.h"
#include "runtime/CanonicalBciRuntimeContract.h"

#include <cassert>
#include <string>
#include <type_traits>

namespace {

dnxr::SignedRuntimeArtifactConfig artifact(const std::string& id) {
    dnxr::SignedRuntimeArtifactConfig a;
    a.artifactId = id;
    a.issuer = "vendor-calibration-authority";
    a.subjectId = "patient-nonhuman-001";
    a.deviceId = "device-attested-001";
    a.schema = "dnxr.runtime-artifact.v1";
    a.compatibility = "fw=fw-1;protocol=proto-1;schema=1";
    a.payloadDigest = dnxr::CryptoSignatureVerifier::digestHex(id + "|payload|nonhuman");
    a.keyId = "vendor-key-1";
    a.trustAnchor = "vendor-root";
    a.validFromUnixSeconds = 1;
    a.validUntilUnixSeconds = 2000;
    a.schemaVersion = 1;
    a.auditId = id + "-audit";
    dnxr::SignedEvidenceDocument doc{a.artifactId,
                                     a.payloadDigest,
                                     "",
                                     a.issuer,
                                     "runtime-artifact-issuer",
                                     a.keyId,
                                     a.validFromUnixSeconds,
                                     a.validUntilUnixSeconds};
    a.signature = dnxr::CryptoSignatureVerifier::digestHex(
        dnxr::CryptoSignatureVerifier::canonicalPayload(doc) + "|" + a.trustAnchor);
    return a;
}

dnxr::RuntimeConfig realConfig() {
    dnxr::RuntimeConfig config;
    config.profile = "real_minimal";
    config.runtimeMode = "real_mode";
    config.simulationMode = false;
    config.virtualWorldIngress.endpoint = "structured://world";
    config.neuralChipTransport.transport = "vendor_attested_transport";
    config.runtimeArtifacts.patientId = "patient-nonhuman-001";
    config.runtimeArtifacts.deviceId = "device-attested-001";
    config.runtimeArtifacts.firmwareVersion = "fw-1";
    config.runtimeArtifacts.protocolVersion = "proto-1";
    config.runtimeArtifacts.calibration = artifact("calibration");
    config.runtimeArtifacts.patientMap = artifact("patient_map");
    config.runtimeArtifacts.topology = artifact("topology");
    config.runtimeArtifacts.capability = artifact("capability");
    return config;
}

dnxr::RuntimeHardwareEvidence evidence() {
    dnxr::RuntimeHardwareEvidence e{};
    e.deviceDiscovered = true;
    e.hardwareIdentityVerified = true;
    e.firmwareVerified = true;
    e.deviceCertificateVerified = true;
    e.capabilitiesNegotiated = true;
    e.readChannelVerified = true;
    e.feedbackChannelVerified = true;
    e.hardwareCutoffVerified = true;
    e.clocksSynchronized = true;
    e.authenticatedSessionCreated = true;
    return e;
}

dnxr::canonical::Digest256 d(std::uint64_t v) { dnxr::canonical::Digest256 out{}; out.words[0] = v; return out; }
dnxr::canonical::TimeEvidence t() { return {dnxr::canonical::MonotonicNs{1}, dnxr::canonical::DeadlineNs{dnxr::canonical::MonotonicNs{2}}, dnxr::canonical::AgeNs{dnxr::canonical::DurationNs{0}}, dnxr::canonical::SkewNs{0}, dnxr::canonical::BudgetNs{dnxr::canonical::DurationNs{1}}}; }

} // namespace

static_assert(!std::is_assignable<dnxr::SignedRuntimeArtifactConfig&, dnxr::SignedRuntimeArtifactConfig>::value == false,
              "artifact configs are plain config records; bootstrap snapshots freeze the digests");

int main() {
    dnxr::SessionBootstrapValidator validator;
    const auto accepted = validator.validate(realConfig(), evidence());
    assert(accepted.ok());
    assert(accepted.snapshot().patientMapDigest.value.present());
    assert(accepted.snapshot().topologyDigest.value.present());
    assert(accepted.snapshot().capabilityDigest.value.present());

    auto badSignature = realConfig();
    badSignature.runtimeArtifacts.patientMap.signature = dnxr::CryptoSignatureVerifier::digestHex("tampered");
    const auto badSignatureRejected = validator.validate(badSignature, evidence());
    assert(!badSignatureRejected.ok());
    assert(badSignatureRejected.error().reason == "patient_map_cryptographic_signature_mismatch");

    auto wrongPatient = realConfig();
    wrongPatient.runtimeArtifacts.topology.subjectId = "other-patient";
    const auto wrongPatientRejected = validator.validate(wrongPatient, evidence());
    assert(!wrongPatientRejected.ok());
    assert(wrongPatientRejected.error().reason == "topology_patient_mismatch");

    auto expired = realConfig();
    expired.runtimeArtifacts.capability.validUntilUnixSeconds = 999;
    const auto expiredRejected = validator.validate(expired, evidence());
    assert(!expiredRejected.ok());
    assert(expiredRejected.error().reason == "capability_validity_window_invalid");

    auto revoked = realConfig();
    revoked.runtimeArtifacts.calibration.revoked = true;
    const auto revokedRejected = validator.validate(revoked, evidence());
    assert(!revokedRejected.ok());
    assert(revokedRejected.error().reason == "calibration_revoked");

    auto reloadable = realConfig();
    reloadable.runtimeArtifacts.immutableAtBootstrap = false;
    const auto reloadableRejected = validator.validate(reloadable, evidence());
    assert(!reloadableRejected.ok());
    assert(reloadableRejected.error().reason == "artifact_hot_reload_forbidden");

    using namespace dnxr::canonical;
    const SessionContext session{SessionId{1}, ConfigurationDigest{d(1)}, PatientMapDigest{d(0)}, TopologyDigest{d(3)}, CapabilityDigest{d(4)}, SafetyPolicyDigest{d(5)}, MonotonicNs{1}};
    const WorldIngressFrame world{session, FrameId{10}, WorldSequence{1}, t(), TransactionDigest{d(6)}};
    const BciAcquisitionFrame acq{world, AcquisitionSequence{1}, t(), TransactionDigest{d(7)}};
    const ConditionedSignal signal{acq, TransactionDigest{d(8)}};
    const InferenceResult inf{signal, TransactionDigest{d(9)}};
    const MotorIntent motor{inf, TransactionDigest{d(10)}};
    const CalibrationSnapshotRef cal{motor, PatientMapDigest{d(0)}, CapabilityDigest{d(4)}};
    const EncodedModalities enc{cal, TransactionDigest{d(11)}};
    const AlignedSpikePlan plan{enc, TopologyDigest{d(3)}, TransactionDigest{d(12)}};
    const SafetyDecision safe{plan, SafetyPolicyDigest{d(5)}, true};
    const auto prepared = prepareCommit(safe, CommitId{1}, PacketNonce{1}, TransactionDigest{d(13)});
    assert(!prepared.ok());
    assert(std::string(prepared.error().reason) == "missing_required_artifact_digest");
}
