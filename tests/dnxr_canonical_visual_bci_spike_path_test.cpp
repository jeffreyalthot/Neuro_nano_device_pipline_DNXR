#include "full_dive/spike_bridge/StimulusBySpikeRuntime.h"
#include "full_dive/visual_chip/CanonicalVisualBciEncoder.h"
#include "evidence/CryptoSignatureVerifier.h"
#include "full_dive/visual_chip/PhospheneFrameStabilizer.h"
#include "full_dive/visual_chip/VisualCorticalWorldEncoder.h"
#include "full_dive/visual_chip/VisualSceneSemanticExtractor.h"
#include "full_dive/visual_chip/VisualVectorToSpikeAdapter.h"
#include <cassert>
#include <cstring>
#include <string>

namespace {
std::string digest(const std::string& input) {
    return dnxr::CryptoSignatureVerifier::digestHex(input);
}

dnxr::CanonicalVirtualWorldFrame canonicalWorld() {
    dnxr::VirtualWorldIngressFrame raw;
    raw.frameId = 77;
    raw.timestampMs = 123.0;
    raw.view.exposure = 0.40f;
    raw.view.contrast = 0.35f;
    raw.view.saturation = 0.70f;
    raw.view.fieldOfViewDeg = 90.0f;
    raw.visualField.nearDepthLayer = 0.45f;
    raw.visualField.midDepthLayer = 0.70f;
    raw.visualField.farDepthLayer = 0.80f;
    raw.visualField.peripheralBlur = 0.70f;
    raw.visualField.motionComfort = 0.80f;
    raw.visualField.fovealRadius = 0.25f;
    raw.visualField.peripheralAwareness = 0.70f;
    raw.visualField.retinalStability = 0.70f;
    raw.body.handConfidence01 = 0.30f;
    raw.body.avatarOwnership01 = 0.30f;
    raw.assets.push_back({"visual-fixture-scene", digest("visual-fixture-scene"), 4096U, true});
    raw.events.push_back({"navigation_label", 0.25f});
    auto world = dnxr::makeCanonicalVirtualWorldFrame(raw, "deterministic_engine_test");
    world.metadata.authenticationTag = "auth-tag";
    world.metadata.messageDigest = digest("canonical-visual-world-message");
    world.metadata.sessionId = "session-visual-test";
    return world;
}

dnxr::CanonicalVisualBciProfile visualProfile(const dnxr::PatientVisualMap& map, const dnxr::VisualSafetyState& safety) {
    dnxr::CanonicalVisualBciProfile profile;
    profile.patientMap = map;
    profile.safety = safety;
    profile.visualModalityAllowed = true;
    profile.signedPatientMap = true;
    profile.signedTopology = true;
    profile.signedCapability = true;
    profile.patientMapDigest = 11U;
    profile.topologyDigest = 22U;
    profile.capabilityDigest = 33U;
    profile.capabilities.canVisual = true;
    profile.capabilities.electrodeCount = 64U;
    profile.capabilities.wireCount = 64U;
    profile.capabilities.maxSafeRateHz = 60.0f;
    profile.capabilities.supportsHardwareCutoff = true;
    profile.capabilities.supportsClosedLoopFeedback = true;
    return profile;
}
} // namespace

int main() {
    dnxr::VirtualSceneFrame scene;
    scene.luminance01 = 0.40f;
    scene.contrast01 = 0.35f;
    scene.edgeDensity01 = 0.30f;
    scene.motionFlow01 = 0.20f;
    scene.nearObstacleDensity01 = 0.45f;
    scene.walkableCorridorConfidence01 = 0.70f;
    scene.horizonStability01 = 0.70f;
    scene.textSymbol01 = 0.25f;
    scene.bodyVisibility01 = 0.30f;
    scene.worldGazeAttention01 = 0.60f;

    const auto semantics = dnxr::VisualSceneSemanticExtractor{}.extract(scene);
    assert(semantics.nearObstacles01 == 0.45f);
    assert(semantics.gazeDirectionConfidence01 == 0.80f);

    dnxr::PatientVisualMap map;
    map.phospheneDensityLimit01 = 0.50f;
    map.stability01 = 0.75f;
    map.retinotopicConfidence01 = 0.90f;
    map.topology.gridColumns = 32;
    map.topology.gridRows = 16;
    map.topology.onOffChannelsEnabled = true;
    map.topology.chromaticChannelsEnabled = true;
    map.topology.foveationStrength01 = 0.65f;
    map.topology.surroundSuppression01 = 0.20f;
    map.topology.compressionRatio01 = 0.25f;
    assert(map.topology.validate());

    dnxr::VisualSafetyState safety;
    safety.edgeGain = 1.2f;

    const auto canonicalAccepted = dnxr::CanonicalVisualBciEncoder{}.encode(canonicalWorld(), visualProfile(map, safety));
    assert(canonicalAccepted.accepted);
    assert(!canonicalAccepted.holdRequired);
    assert(canonicalAccepted.executedPhaseCount == static_cast<std::uint8_t>(dnxr::CanonicalVisualBciPhase::Count));
    assert(canonicalAccepted.frameId == 77U);
    assert(canonicalAccepted.traceDigest != 0U);
    assert(canonicalAccepted.spikeCandidates.modality == dnxr::SpikeModality::Visual);
    assert(canonicalAccepted.spikeCandidates.channelCount == 16U);
    assert(std::strcmp(canonicalAccepted.reason, "visual_bci_spike_path_accept") == 0);
    const auto canonicalReplay = dnxr::CanonicalVisualBciEncoder{}.encode(canonicalWorld(), visualProfile(map, safety));
    assert(canonicalReplay.traceDigest == canonicalAccepted.traceDigest);
    assert(canonicalReplay.spikeCandidates.channels01[0] == canonicalAccepted.spikeCandidates.channels01[0]);

    auto invalidAssetWorld = canonicalWorld();
    invalidAssetWorld.modalities.assets[0].sha256Digest = "bad";
    const auto invalidAsset = dnxr::CanonicalVisualBciEncoder{}.encode(invalidAssetWorld, visualProfile(map, safety));
    assert(!invalidAsset.accepted);
    assert(invalidAsset.holdRequired);
    assert(std::strcmp(invalidAsset.reason, "visual_asset_metadata_invalid") == 0);

    auto badProfile = visualProfile(map, safety);
    badProfile.signedTopology = false;
    const auto unsignedTopology = dnxr::CanonicalVisualBciEncoder{}.encode(canonicalWorld(), badProfile);
    assert(!unsignedTopology.accepted);
    assert(std::strcmp(unsignedTopology.reason, "visual_required_signed_artifact_missing") == 0);

    auto noCapability = visualProfile(map, safety);
    noCapability.capabilities.canVisual = false;
    const auto capabilityHold = dnxr::CanonicalVisualBciEncoder{}.encode(canonicalWorld(), noCapability);
    assert(!capabilityHold.accepted);
    assert(std::strcmp(capabilityHold.reason, "visual_capability_contract_denied") == 0);

    auto badProjection = canonicalWorld();
    badProjection.modalities.visualField.horizontalFovDeg = -1.0f;
    const auto projectionHold = dnxr::CanonicalVisualBciEncoder{}.encode(badProjection, visualProfile(map, safety));
    assert(!projectionHold.accepted);
    assert(std::strcmp(projectionHold.reason, "visual_projection_metadata_invalid") == 0);

    auto visual = dnxr::VisualCorticalWorldEncoder{}.encode(scene, map, safety);
    visual = dnxr::PhospheneFrameStabilizer{}.stabilize(visual);
    const auto compliance = dnxr::VisualCorticalWorldEncoder{}.validateEncoded(visual);
    assert(compliance.accepted);
    assert(visual.depthNear01 == 0.45f);
    assert(visual.phospheneDensity01 > 0.0f && visual.phospheneDensity01 <= dnxr::DNXR_NEURAL_CHIP_ACTIVE_CEILING_01);

    const auto packet = dnxr::VisualVectorToSpikeAdapter{}.adapt(77, 123.0, visual);
    assert(packet.modality == dnxr::SpikeModality::Visual);
    assert(packet.channelCount == 16U);
    assert(std::strcmp(packet.sourceEncoder, "VisualCorticalWorldEncoder") == 0);

    dnxr::StimulusBySpikeInput input;
    input.frameId = 77;
    input.timestampMs = 123.0;
    input.visual = visual;
    input.realityAnchor = {};
    input.patientMap = dnxr::PatientSpecificNeuralMap::simulationDefaults();
    input.patientMap.visual.enabled = true;
    input.patientMap.visual.confidence01 = 0.95f;
    input.patientMap.visual.maxAllowed01 = 0.80f;
    input.patientMap.visualChip.retinotopicConfidence01 = 0.90f;
    input.safety.simulationOnly = true;
    input.lastAck.packetAccepted = true;
    input.lastAck.signalQuality = 1.0f;
    input.sessionState.realityAnchorStability01 = 1.0f;
    const auto a = dnxr::StimulusBySpikeRuntime{}.process(input);
    const auto b = dnxr::StimulusBySpikeRuntime{}.process(input);
    assert(a.stageCoverageComplete);
    assert(a.pppGrammarFrame.modalityCount > 0U);
    assert(a.pppGrammarFrame.modalities[0].modalityId == 0x01);
    assert(a.engineSpike.laneCount > 0U);
    assert(a.engineSpike.totalSpikeLoad01 <= dnxr::DNXR_NEURAL_CHIP_ACTIVE_CEILING_01);
    assert(a.stimulusBySpike.channelCount > 0U);
    assert(a.stimulusBySpike.boundedChannels01[0] == b.stimulusBySpike.boundedChannels01[0]);

    dnxr::VisualSafetyState flashSafety;
    flashSafety.flickerRisk01 = 0.90f;
    const auto flash = dnxr::VisualCorticalWorldEncoder{}.encode(scene, map, flashSafety);
    assert(!dnxr::VisualCorticalWorldEncoder{}.validateEncoded(flash).accepted);

    map.topology.gridColumns = 128;
    map.topology.gridRows = 128;
    assert(!map.topology.validate());
    assert(dnxr::VisualCorticalWorldEncoder{}.encode(scene, map, safety).phospheneDensity01 == 0.0f);

    safety.allowVisualStimulus = false;
    const auto neutral = dnxr::VisualCorticalWorldEncoder{}.encode(scene, {}, safety);
    assert(neutral.globalLuminance01 == 0.0f);
    assert(neutral.phospheneDensity01 == 0.0f);
    return 0;
}
