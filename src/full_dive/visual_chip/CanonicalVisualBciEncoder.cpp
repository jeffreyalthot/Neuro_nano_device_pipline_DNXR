#include "full_dive/visual_chip/CanonicalVisualBciEncoder.h"

#include <algorithm>
#include <cmath>
#include <cstring>

namespace dnxr {
namespace {
constexpr std::uint64_t kFnvOffset = 1469598103934665603ULL;
constexpr std::uint32_t kPhaseBudgetsUs[] = {120U, 90U, 160U, 220U, 140U, 140U, 180U};
constexpr const char* kPhaseNames[] = {
    "scene_validation",
    "projection_reference",
    "semantic_feature_extraction",
    "spatial_temporal_encoding",
    "patient_capability_mapping",
    "safety_normalization",
    "spike_candidate_generation"};

bool finite(float value) noexcept { return std::isfinite(value); }
bool finiteDouble(double value) noexcept { return std::isfinite(value); }

std::uint64_t strMix(std::uint64_t seed, const std::string& value) noexcept {
    std::uint64_t out = seed;
    for (const unsigned char c : value) {
        out ^= c;
        out *= 1099511628211ULL;
    }
    return out;
}
} // namespace

float CanonicalVisualBciEncoder::c01(float value) noexcept {
    return std::max(0.0f, std::min(1.0f, value));
}

std::uint64_t CanonicalVisualBciEncoder::mix(std::uint64_t seed, std::uint64_t value) noexcept {
    std::uint64_t out = seed ^ value;
    out *= 1099511628211ULL;
    out ^= out >> 32U;
    return out;
}

std::uint64_t CanonicalVisualBciEncoder::mixFloat(std::uint64_t seed, float value) noexcept {
    const auto quantized = static_cast<std::uint64_t>(std::llround(c01(value) * 1000000.0f));
    return mix(seed, quantized);
}

bool CanonicalVisualBciEncoder::capabilityAllowsVisual(const ChipCapabilities& capabilities) noexcept {
    return capabilities.canVisual && capabilities.electrodeCount > 0U && capabilities.wireCount > 0U &&
           capabilities.maxSafeRateHz > 0.0f && capabilities.supportsHardwareCutoff &&
           capabilities.supportsClosedLoopFeedback;
}

bool CanonicalVisualBciEncoder::profileValid(const CanonicalVisualBciProfile& profile, const char** reason) noexcept {
    if (!profile.visualModalityAllowed) { *reason = "visual_modality_not_allowed"; return false; }
    if (!profile.signedPatientMap || !profile.signedTopology || !profile.signedCapability) {
        *reason = "visual_required_signed_artifact_missing"; return false;
    }
    if (profile.patientMapDigest == 0U || profile.topologyDigest == 0U || profile.capabilityDigest == 0U) {
        *reason = "visual_required_artifact_digest_missing"; return false;
    }
    if (!profile.patientMap.topology.validate()) { *reason = "visual_topology_invalid"; return false; }
    if (profile.patientMap.retinotopicConfidence01 <= 0.0f) { *reason = "visual_patient_map_unbound"; return false; }
    if (!capabilityAllowsVisual(profile.capabilities)) { *reason = "visual_capability_contract_denied"; return false; }
    return true;
}

bool CanonicalVisualBciEncoder::metadataValid(const CanonicalVirtualWorldFrame& world, const char** reason) noexcept {
    if (!world.modalities.visualPresent) { *reason = "visual_world_modality_absent"; return false; }
    if (world.metadata.schemaVersion != CanonicalVirtualWorldMetadata::kCurrentSchemaVersion) {
        *reason = "visual_world_schema_mismatch"; return false;
    }
    if (world.metadata.sequenceId == 0U || !finiteDouble(world.metadata.engineTimestampMs) || world.metadata.engineTimestampMs <= 0.0) {
        *reason = "visual_world_timestamp_invalid"; return false;
    }
    if (world.metadata.engineProvenance.empty() || world.metadata.messageDigest.size() != 64U ||
        world.metadata.authenticationTag.empty()) {
        *reason = "visual_world_provenance_or_digest_missing"; return false;
    }
    const auto& view = world.modalities.view;
    const auto& field = world.modalities.visualField;
    if (!finite(view.fieldOfViewDeg) || view.fieldOfViewDeg < 30.0f || view.fieldOfViewDeg > 140.0f ||
        !finite(view.exposure) || !finite(view.contrast) || !finite(view.saturation) ||
        !finite(field.horizontalFovDeg) || !finite(field.verticalFovDeg) ||
        field.horizontalFovDeg <= 0.0f || field.verticalFovDeg <= 0.0f ||
        !finite(field.nearDepthLayer) || !finite(field.midDepthLayer) || !finite(field.farDepthLayer) ||
        field.nearDepthLayer > field.midDepthLayer || field.midDepthLayer > field.farDepthLayer ||
        !finite(field.retinalStability)) {
        *reason = "visual_projection_metadata_invalid"; return false;
    }
    for (const auto& asset : world.modalities.assets) {
        if (!asset.resolved || asset.assetId.empty() || asset.sha256Digest.size() != 64U || asset.byteSize == 0U) {
            *reason = "visual_asset_metadata_invalid"; return false;
        }
    }
    return true;
}

VirtualSceneFrame CanonicalVisualBciEncoder::sceneFromWorld(const CanonicalVirtualWorldFrame& world) noexcept {
    VirtualSceneFrame scene{};
    const auto& view = world.modalities.view;
    const auto& field = world.modalities.visualField;
    scene.luminance01 = c01(view.exposure);
    scene.contrast01 = c01(view.contrast);
    scene.edgeDensity01 = c01(1.0f - field.peripheralBlur);
    scene.motionFlow01 = c01(1.0f - field.motionComfort);
    scene.nearObstacle01 = c01(field.nearDepthLayer);
    scene.midDepthStructure01 = c01(field.midDepthLayer);
    scene.farDepthStructure01 = c01(field.farDepthLayer);
    scene.salientObject01 = c01(field.fovealRadius);
    scene.navigationCorridor01 = c01(field.peripheralAwareness);
    scene.hazard01 = c01(std::max(field.vignetteStrength, world.modalities.environment.navigationRisk01));
    scene.nearObstacleDensity01 = c01(field.nearDepthLayer);
    scene.walkableCorridorConfidence01 = c01(world.modalities.body.footConfidence01);
    scene.horizonStability01 = c01(field.retinalStability);
    scene.interactiveObjectPriority01 = c01(std::min(1.0f, static_cast<float>(world.modalities.events.size()) * 0.10f));
    scene.handVisibility01 = c01(world.modalities.body.handConfidence01);
    scene.bodyVisibility01 = c01(world.modalities.body.avatarOwnership01);
    scene.avatarBody01 = c01(world.modalities.body.avatarOwnership01);
    scene.worldGazeAttention01 = c01(1.0f - field.peripheralBlur);
    scene.textSymbol01 = 0.0f;
    for (const auto& event : world.modalities.events) {
        if (event.eventType == "text" || event.eventType == "symbol" || event.eventType == "navigation_label") {
            scene.textSymbol01 = std::max(scene.textSymbol01, c01(event.priority01));
        }
    }
    return scene;
}

CanonicalVisualBciResult CanonicalVisualBciEncoder::encode(const CanonicalVirtualWorldFrame& world,
                                                           const CanonicalVisualBciProfile& profile) const {
    CanonicalVisualBciResult result{};
    result.frameId = world.metadata.sequenceId;
    result.traceDigest = mix(kFnvOffset, result.frameId);
    auto mark = [&](CanonicalVisualBciPhase phase, bool accepted, const char* reason, float metric) {
        const auto index = static_cast<std::size_t>(phase);
        result.phases[index] = {phase, kPhaseNames[index], reason, kPhaseBudgetsUs[index], accepted, c01(metric)};
        result.executedPhaseCount = static_cast<std::uint8_t>(std::max<std::size_t>(result.executedPhaseCount, index + 1U));
        result.traceDigest = strMix(mixFloat(mix(result.traceDigest, static_cast<std::uint64_t>(index + 1U)), metric), reason ? reason : "none");
    };

    const char* reason = "visual_accept";
    if (!metadataValid(world, &reason)) {
        mark(CanonicalVisualBciPhase::SceneValidation, false, reason, 1.0f);
        result.reason = reason;
        return result;
    }
    mark(CanonicalVisualBciPhase::SceneValidation, true, "visual_scene_metadata_valid", 0.0f);

    const VirtualSceneFrame scene = sceneFromWorld(world);
    mark(CanonicalVisualBciPhase::ProjectionReference, true, "visual_world_projection_canonicalized", scene.motionFlow01);

    result.semantics = VisualSceneSemanticExtractor{}.extract(scene);
    mark(CanonicalVisualBciPhase::SemanticFeatureExtraction, true, "visual_semantics_extracted", result.semantics.visualDanger01);

    result.visual = VisualCorticalWorldEncoder{}.encode(scene, profile.patientMap, profile.safety);
    mark(CanonicalVisualBciPhase::SpatialTemporalEncoding, true, "visual_spatial_temporal_encoded", result.visual.phospheneDensity01);

    if (!profileValid(profile, &reason)) {
        result.visual = NeuralVisualVector{};
        mark(CanonicalVisualBciPhase::PatientCapabilityMapping, false, reason, 1.0f);
        result.reason = reason;
        return result;
    }
    mark(CanonicalVisualBciPhase::PatientCapabilityMapping, true, "visual_patient_capability_mapping_verified", 0.0f);

    result.visual = PhospheneFrameStabilizer{}.stabilize(result.visual);
    const auto compliance = VisualCorticalWorldEncoder{}.validateEncoded(result.visual);
    mark(CanonicalVisualBciPhase::SafetyNormalization, compliance.accepted, compliance.reason, compliance.normalizedLoad01);
    if (!compliance.accepted) {
        result.visual = NeuralVisualVector{};
        result.reason = compliance.reason;
        return result;
    }

    result.spikeCandidates = VisualVectorToSpikeAdapter{}.adapt(world.metadata.sequenceId, world.metadata.engineTimestampMs, result.visual);
    const bool spikeOk = result.spikeCandidates.modality == SpikeModality::Visual && result.spikeCandidates.channelCount > 0U;
    mark(CanonicalVisualBciPhase::SpikeCandidateGeneration, spikeOk, spikeOk ? "visual_spike_candidates_ready" : "visual_spike_candidate_generation_failed", result.spikeCandidates.confidence01);
    result.accepted = spikeOk;
    result.holdRequired = !spikeOk;
    result.reason = spikeOk ? "visual_bci_spike_path_accept" : "visual_spike_candidate_generation_failed";
    return result;
}

} // namespace dnxr
