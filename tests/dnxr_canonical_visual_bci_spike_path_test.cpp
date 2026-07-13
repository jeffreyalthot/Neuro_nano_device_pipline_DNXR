#include "full_dive/spike_bridge/StimulusBySpikeRuntime.h"
#include "full_dive/visual_chip/PhospheneFrameStabilizer.h"
#include "full_dive/visual_chip/VisualCorticalWorldEncoder.h"
#include "full_dive/visual_chip/VisualSceneSemanticExtractor.h"
#include "full_dive/visual_chip/VisualVectorToSpikeAdapter.h"
#include <cassert>
#include <cstring>

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
