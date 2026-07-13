#include "full_dive/auditory_chip/CanonicalAuditoryBciEncoder.h"
#include "evidence/CryptoSignatureVerifier.h"

#include <cassert>
#include <cstring>
#include <string>

namespace {
std::string digest(const std::string& input) {
    return dnxr::CryptoSignatureVerifier::digestHex(input);
}

dnxr::CanonicalVirtualWorldFrame auditoryWorld() {
    dnxr::VirtualWorldIngressFrame raw;
    raw.frameId = 88;
    raw.timestampMs = 456.0;
    raw.audio.volume01 = 0.30f;
    raw.audio.lowFrequency01 = 0.15f;
    raw.audio.midFrequency01 = 0.25f;
    raw.audio.highFrequency01 = 0.10f;
    raw.audio.attack01 = 0.05f;
    raw.audio.sustain01 = 0.20f;
    raw.audio.directionLeftRight01 = 0.60f;
    raw.audio.directionFrontBack01 = 0.70f;
    raw.audio.sourceDistance01 = 0.25f;
    raw.audio.sourceVelocity01 = 0.10f;
    raw.audio.roomSize01 = 0.40f;
    raw.audio.reverberation01 = 0.15f;
    raw.audio.speechPresence01 = 0.35f;
    raw.audio.dangerPriority01 = 0.05f;
    raw.audio.sampleRateHz = 48000;
    raw.audio.channelCount = 2;
    raw.audio.assetId = "audio-fixture-wave";
    raw.audio.assetDigest = digest("audio-fixture-wave");
    auto world = dnxr::makeCanonicalVirtualWorldFrame(raw, "deterministic_audio_engine_test");
    world.metadata.authenticationTag = "audio-auth-tag";
    world.metadata.messageDigest = digest("canonical-auditory-world-message");
    world.metadata.sessionId = "session-auditory-test";
    return world;
}

dnxr::CanonicalAuditoryBciProfile auditoryProfile() {
    dnxr::CanonicalAuditoryBciProfile profile;
    profile.patientMap.tonotopicGain01 = 0.85f;
    profile.safety.allowAuditoryStimulus = true;
    profile.auditoryModalityAllowed = true;
    profile.signedPatientMap = true;
    profile.signedTopology = true;
    profile.signedCapability = true;
    profile.phaseLockVerified = true;
    profile.isolationVerified = true;
    profile.dosePolicyVerified = true;
    profile.patientMapDigest = 101U;
    profile.topologyDigest = 202U;
    profile.capabilityDigest = 303U;
    profile.capabilities.canAuditory = true;
    profile.capabilities.electrodeCount = 64U;
    profile.capabilities.wireCount = 64U;
    profile.capabilities.maxSafeRateHz = 60.0f;
    profile.capabilities.supportsHardwareCutoff = true;
    profile.capabilities.supportsClosedLoopFeedback = true;
    return profile;
}
} // namespace

int main() {
    const auto accepted = dnxr::CanonicalAuditoryBciEncoder{}.encode(auditoryWorld(), auditoryProfile());
    assert(accepted.accepted);
    assert(!accepted.holdRequired);
    assert(accepted.executedPhaseCount == static_cast<std::uint8_t>(dnxr::CanonicalAuditoryBciPhase::Count));
    assert(accepted.frameId == 88U);
    assert(accepted.traceDigest != 0U);
    assert(accepted.spikeCandidates.modality == dnxr::SpikeModality::Auditory);
    assert(accepted.spikeCandidates.channelCount > 0U);
    assert(std::strcmp(accepted.spikeCandidates.sourceEncoder, "AuditoryTonotopicEncoder") == 0);
    assert(std::strcmp(accepted.reason, "auditory_bci_spike_path_accept") == 0);

    const auto replay = dnxr::CanonicalAuditoryBciEncoder{}.encode(auditoryWorld(), auditoryProfile());
    assert(replay.traceDigest == accepted.traceDigest);
    assert(replay.spikeCandidates.channels01[0] == accepted.spikeCandidates.channels01[0]);

    auto badRate = auditoryWorld();
    badRate.modalities.audio.sampleRateHz = 44100;
    const auto badRateResult = dnxr::CanonicalAuditoryBciEncoder{}.encode(badRate, auditoryProfile());
    assert(!badRateResult.accepted);
    assert(std::strcmp(badRateResult.reason, "auditory_sample_rate_unsupported") == 0);

    auto badDigest = auditoryWorld();
    badDigest.modalities.audio.assetDigest = "bad";
    const auto badDigestResult = dnxr::CanonicalAuditoryBciEncoder{}.encode(badDigest, auditoryProfile());
    assert(!badDigestResult.accepted);
    assert(std::strcmp(badDigestResult.reason, "auditory_asset_digest_invalid") == 0);

    auto discontinuity = auditoryWorld();
    discontinuity.modalities.audio.discontinuity01 = 0.90f;
    const auto discontinuityResult = dnxr::CanonicalAuditoryBciEncoder{}.encode(discontinuity, auditoryProfile());
    assert(!discontinuityResult.accepted);
    assert(std::strcmp(discontinuityResult.reason, "auditory_discontinuity_hold") == 0);

    auto packetLoss = auditoryWorld();
    packetLoss.modalities.audio.packetLoss01 = 0.40f;
    const auto packetLossResult = dnxr::CanonicalAuditoryBciEncoder{}.encode(packetLoss, auditoryProfile());
    assert(!packetLossResult.accepted);
    assert(std::strcmp(packetLossResult.reason, "auditory_packet_loss_or_clock_drift_hold") == 0);

    auto noPhaseLock = auditoryProfile();
    noPhaseLock.phaseLockVerified = false;
    const auto noPhaseLockResult = dnxr::CanonicalAuditoryBciEncoder{}.encode(auditoryWorld(), noPhaseLock);
    assert(!noPhaseLockResult.accepted);
    assert(std::strcmp(noPhaseLockResult.reason, "auditory_phase_lock_missing") == 0);

    auto noCapability = auditoryProfile();
    noCapability.capabilities.canAuditory = false;
    const auto noCapabilityResult = dnxr::CanonicalAuditoryBciEncoder{}.encode(auditoryWorld(), noCapability);
    assert(!noCapabilityResult.accepted);
    assert(std::strcmp(noCapabilityResult.reason, "auditory_capability_contract_denied") == 0);

    auto clipping = auditoryWorld();
    clipping.modalities.audio.clippingRisk01 = 0.85f;
    const auto clippingResult = dnxr::CanonicalAuditoryBciEncoder{}.encode(clipping, auditoryProfile());
    assert(!clippingResult.accepted);
    assert(std::strcmp(clippingResult.reason, "auditory_startle_or_clipping_hold") == 0);

    auto silence = auditoryWorld();
    silence.modalities.audio.volume01 = 0.0f;
    const auto silenceResult = dnxr::CanonicalAuditoryBciEncoder{}.encode(silence, auditoryProfile());
    assert(silenceResult.accepted);
    assert(silenceResult.auditory.loudness01 == 0.0f);
}
