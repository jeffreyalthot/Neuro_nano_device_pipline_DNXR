#include "full_dive/auditory_chip/CanonicalAuditoryBciEncoder.h"

#include <algorithm>
#include <cmath>

namespace dnxr {
namespace {
constexpr std::uint64_t kFnvOffset = 1469598103934665603ULL;
constexpr std::uint32_t kPhaseBudgetsUs[] = {120U, 120U, 180U, 200U, 140U, 160U, 180U};
constexpr const char* kPhaseNames[] = {
    "media_validation",
    "deterministic_segmentation",
    "semantic_spatialization",
    "tonotopic_envelope_mapping",
    "patient_capability_mapping",
    "safety_phase_normalization",
    "spike_candidate_generation"};

bool finite(float value) noexcept { return std::isfinite(value); }
bool finiteDouble(double value) noexcept { return std::isfinite(value); }

std::uint64_t strMix(std::uint64_t seed, const char* value) noexcept {
    std::uint64_t out = seed;
    if (!value) return out;
    while (*value) {
        out ^= static_cast<unsigned char>(*value++);
        out *= 1099511628211ULL;
    }
    return out;
}
} // namespace

float CanonicalAuditoryBciEncoder::c01(float value) noexcept {
    return std::max(0.0f, std::min(1.0f, value));
}

std::uint64_t CanonicalAuditoryBciEncoder::mix(std::uint64_t seed, std::uint64_t value) noexcept {
    std::uint64_t out = seed ^ value;
    out *= 1099511628211ULL;
    out ^= out >> 32U;
    return out;
}

std::uint64_t CanonicalAuditoryBciEncoder::mixFloat(std::uint64_t seed, float value) noexcept {
    return mix(seed, static_cast<std::uint64_t>(std::llround(c01(value) * 1000000.0f)));
}

bool CanonicalAuditoryBciEncoder::capabilityAllowsAuditory(const ChipCapabilities& capabilities) noexcept {
    return capabilities.canAuditory && capabilities.electrodeCount > 0U && capabilities.wireCount > 0U &&
           capabilities.maxSafeRateHz > 0.0f && capabilities.supportsHardwareCutoff &&
           capabilities.supportsClosedLoopFeedback;
}

bool CanonicalAuditoryBciEncoder::profileValid(const CanonicalAuditoryBciProfile& profile, const char** reason) noexcept {
    if (!profile.auditoryModalityAllowed) { *reason = "auditory_modality_not_allowed"; return false; }
    if (!profile.signedPatientMap || !profile.signedTopology || !profile.signedCapability) {
        *reason = "auditory_required_signed_artifact_missing"; return false;
    }
    if (profile.patientMapDigest == 0U || profile.topologyDigest == 0U || profile.capabilityDigest == 0U) {
        *reason = "auditory_required_artifact_digest_missing"; return false;
    }
    if (!profile.phaseLockVerified) { *reason = "auditory_phase_lock_missing"; return false; }
    if (!profile.isolationVerified) { *reason = "auditory_isolation_missing"; return false; }
    if (!profile.dosePolicyVerified) { *reason = "auditory_dose_policy_missing"; return false; }
    if (!capabilityAllowsAuditory(profile.capabilities)) { *reason = "auditory_capability_contract_denied"; return false; }
    return true;
}

bool CanonicalAuditoryBciEncoder::mediaMetadataValid(const CanonicalVirtualWorldFrame& world, const char** reason) noexcept {
    if (!world.modalities.audioPresent) { *reason = "auditory_world_modality_absent"; return false; }
    if (world.metadata.schemaVersion != CanonicalVirtualWorldMetadata::kCurrentSchemaVersion) {
        *reason = "auditory_world_schema_mismatch"; return false;
    }
    if (world.metadata.sequenceId == 0U || !finiteDouble(world.metadata.engineTimestampMs) || world.metadata.engineTimestampMs <= 0.0) {
        *reason = "auditory_world_timestamp_invalid"; return false;
    }
    if (world.metadata.engineProvenance.empty() || world.metadata.messageDigest.size() != 64U ||
        world.metadata.authenticationTag.empty()) {
        *reason = "auditory_world_provenance_or_digest_missing"; return false;
    }
    const auto& audio = world.modalities.audio;
    if (!finite(audio.volume01) || !finite(audio.lowFrequency01) || !finite(audio.midFrequency01) ||
        !finite(audio.highFrequency01) || !finite(audio.attack01) || !finite(audio.sustain01) ||
        !finite(audio.directionLeftRight01) || !finite(audio.directionFrontBack01) ||
        !finite(audio.sourceDistance01) || !finite(audio.sourceVelocity01) || !finite(audio.roomSize01) ||
        !finite(audio.reverberation01) || !finite(audio.clockDrift01) || !finite(audio.packetLoss01)) {
        *reason = "auditory_media_non_finite"; return false;
    }
    if (audio.sampleRateHz != 16000U && audio.sampleRateHz != 24000U && audio.sampleRateHz != 48000U) {
        *reason = "auditory_sample_rate_unsupported"; return false;
    }
    if (audio.channelCount == 0U || audio.channelCount > 2U) { *reason = "auditory_channel_mismatch"; return false; }
    if (audio.format != "semantic_bci_audio_v1" || audio.unitSystem != "normalized01") {
        *reason = "auditory_format_or_units_invalid"; return false;
    }
    if (!audio.assetId.empty() && audio.assetDigest.size() != 64U) {
        *reason = "auditory_asset_digest_invalid"; return false;
    }
    if (audio.packetLoss01 > 0.25f || audio.clockDrift01 > 0.20f) {
        *reason = "auditory_packet_loss_or_clock_drift_hold"; return false;
    }
    return true;
}

SpatialAudioFrame CanonicalAuditoryBciEncoder::spatialize(const CanonicalVirtualWorldFrame& world) noexcept {
    const auto& audio = world.modalities.audio;
    SpatialAudioSource source{"canonical-world-audio", audio.dangerPriority01 > 0.0f ? "danger" : "semantic",
                              1.0f + 10.0f * c01(audio.sourceDistance01),
                              (c01(audio.directionLeftRight01) - 0.5f) * 180.0f,
                              c01(audio.volume01)};
    const auto mix = SpatialAudioEngine{}.synthesizeSpatialMix({source}, 1.0f - c01(audio.clippingRisk01), 1.0f, 1.0f);
    SpatialAudioFrame out{};
    out.loudness01 = c01(audio.volume01 * (1.0f - 0.5f * c01(audio.clippingRisk01)));
    out.lowBand01 = c01(audio.lowFrequency01);
    out.midBand01 = c01(audio.midFrequency01);
    out.highBand01 = c01(audio.highFrequency01);
    out.attack01 = c01(audio.attack01);
    out.sustain01 = c01(audio.sustain01);
    out.leftRight01 = mix.size() >= 2U ? c01((mix[1] - mix[0]) * 0.5f + 0.5f) : c01(audio.directionLeftRight01);
    out.frontBack01 = mix.size() >= 4U ? c01((mix[2] - mix[3]) * 0.5f + 0.5f) : c01(audio.directionFrontBack01);
    out.distance01 = c01(audio.sourceDistance01);
    out.speech01 = c01(audio.speechPresence01);
    out.danger01 = c01(audio.dangerPriority01);
    return out;
}

CanonicalAuditoryBciResult CanonicalAuditoryBciEncoder::encode(const CanonicalVirtualWorldFrame& world,
                                                               const CanonicalAuditoryBciProfile& profile) const {
    CanonicalAuditoryBciResult result{};
    result.frameId = world.metadata.sequenceId;
    result.traceDigest = mix(kFnvOffset, result.frameId);
    auto mark = [&](CanonicalAuditoryBciPhase phase, bool accepted, const char* reason, float metric) {
        const auto index = static_cast<std::size_t>(phase);
        result.phases[index] = {phase, kPhaseNames[index], reason, kPhaseBudgetsUs[index], accepted, c01(metric)};
        result.executedPhaseCount = static_cast<std::uint8_t>(std::max<std::size_t>(result.executedPhaseCount, index + 1U));
        result.traceDigest = strMix(mixFloat(mix(result.traceDigest, static_cast<std::uint64_t>(index + 1U)), metric), reason);
    };

    const char* reason = "auditory_accept";
    if (!mediaMetadataValid(world, &reason)) {
        mark(CanonicalAuditoryBciPhase::MediaValidation, false, reason, 1.0f);
        result.reason = reason;
        return result;
    }
    mark(CanonicalAuditoryBciPhase::MediaValidation, true, "auditory_media_metadata_valid", 0.0f);

    const auto& audio = world.modalities.audio;
    const bool silence = audio.volume01 <= 0.0001f;
    const bool discontinuity = audio.discontinuity01 > 0.65f;
    if (discontinuity) {
        mark(CanonicalAuditoryBciPhase::DeterministicSegmentation, false, "auditory_discontinuity_hold", audio.discontinuity01);
        result.reason = "auditory_discontinuity_hold";
        return result;
    }
    mark(CanonicalAuditoryBciPhase::DeterministicSegmentation, true, silence ? "auditory_silence_segmented_neutral" : "auditory_segmented_10ms_deterministic", silence ? 0.0f : audio.volume01);

    result.spatialFrame = silence ? SpatialAudioFrame{} : spatialize(world);
    mark(CanonicalAuditoryBciPhase::SemanticSpatialization, true, "auditory_semantic_spatialization_complete", result.spatialFrame.loudness01);

    result.auditory = AuditoryTonotopicEncoder{}.encode(result.spatialFrame, profile.patientMap, profile.safety);
    mark(CanonicalAuditoryBciPhase::TonotopicEnvelopeMapping, true, "auditory_tonotopic_envelope_mapped", result.auditory.loudness01);

    if (!profileValid(profile, &reason)) {
        result.auditory = NeuralAuditoryVector{};
        mark(CanonicalAuditoryBciPhase::PatientCapabilityMapping, false, reason, 1.0f);
        result.reason = reason;
        return result;
    }
    mark(CanonicalAuditoryBciPhase::PatientCapabilityMapping, true, "auditory_patient_capability_mapping_verified", 0.0f);

    if (profile.safety.startleRisk01 > 0.70f || audio.clippingRisk01 > 0.70f) {
        result.auditory = NeuralAuditoryVector{};
        mark(CanonicalAuditoryBciPhase::SafetyPhaseNormalization, false, "auditory_startle_or_clipping_hold", std::max(profile.safety.startleRisk01, audio.clippingRisk01));
        result.reason = "auditory_startle_or_clipping_hold";
        return result;
    }
    mark(CanonicalAuditoryBciPhase::SafetyPhaseNormalization, true, "auditory_safety_phase_locked", result.auditory.startleRisk01);

    result.spikeCandidates = AuditoryVectorToSpikeAdapter{}.adapt(world.metadata.sequenceId, world.metadata.engineTimestampMs, result.auditory);
    const bool spikeOk = result.spikeCandidates.modality == SpikeModality::Auditory && result.spikeCandidates.channelCount > 0U;
    mark(CanonicalAuditoryBciPhase::SpikeCandidateGeneration, spikeOk, spikeOk ? "auditory_spike_candidates_ready" : "auditory_spike_candidate_generation_failed", result.spikeCandidates.confidence01);
    result.accepted = spikeOk;
    result.holdRequired = !spikeOk;
    result.reason = spikeOk ? "auditory_bci_spike_path_accept" : "auditory_spike_candidate_generation_failed";
    return result;
}

} // namespace dnxr
