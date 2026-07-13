#include "full_dive/auditory_chip/AuditoryTonotopicEncoder.h"
#include "full_dive/auditory_chip/AuditoryVectorToSpikeAdapter.h"
#include "full_dive/gustatory_chip/GustatoryVectorToSpikeAdapter.h"
#include "full_dive/gustatory_chip/NeuralGustatoryEncoder.h"
#include "full_dive/olfactory_chip/NeuralOlfactoryEncoder.h"
#include "full_dive/olfactory_chip/OlfactoryVectorToSpikeAdapter.h"
#include "full_dive/safety/ExperimentalChemicalChannelGate.h"
#include "full_dive/safety/ModalityExposureLedger.h"
#include "full_dive/safety/PerModalitySafetyEnvelope.h"
#include "full_dive/spike_bridge/StimulusBySpikeRuntime.h"
#include "full_dive/sync/MultisensoryPhaseLockOrchestrator.h"
#include "full_dive/touch_chip/NeuralBodySenseEncoders.h"
#include "full_dive/touch_chip/NeuralVirtualEpidermisEncoder.h"
#include "full_dive/touch_chip/ProprioceptionVectorToSpikeAdapter.h"
#include "full_dive/touch_chip/TouchVectorToSpikeAdapter.h"
#include "full_dive/touch_chip/VestibularVectorToSpikeAdapter.h"
#include "full_dive/visual_chip/VisualCorticalWorldEncoder.h"
#include "neural_chip/ChipCapabilityContract.h"
#include "neural_chip/DisabledModalityNeutralityValidator.h"
#include "neural_chip/PatientModalityPolicyGate.h"

#include <cassert>

namespace {

dnxr::PatientSpecificNeuralMap enabledNonChemicalMap() {
  auto map = dnxr::PatientSpecificNeuralMap::simulationDefaults();
  auto enable = [](dnxr::PatientModalityProfile &profile) {
    profile.enabled = true;
    profile.confidence01 = 0.95f;
    profile.sensitivity01 = 1.0f;
    profile.maxAllowed01 = dnxr::DNXR_NEURAL_CHIP_ACTIVE_CEILING_01;
    profile.maturity = dnxr::NeuralChannelMaturity::SupportedSimulation;
  };
  enable(map.visual);
  enable(map.auditory);
  enable(map.touch);
  enable(map.proprioception);
  enable(map.vestibular);
  enable(map.motor);
  map.olfactory.enabled = false;
  map.gustatory.enabled = false;
  return map;
}

} // namespace

int main() {
  dnxr::SpatialAudioFrame audioFrame;
  audioFrame.loudness01 = 0.30f;
  audioFrame.lowBand01 = 0.15f;
  audioFrame.midBand01 = 0.25f;
  audioFrame.highBand01 = 0.10f;
  audioFrame.attack01 = 0.05f;
  audioFrame.sustain01 = 0.20f;
  audioFrame.leftRight01 = 0.60f;
  audioFrame.frontBack01 = 0.70f;
  audioFrame.speech01 = 0.35f;
  dnxr::AuditorySafetyState auditorySafety;
  const auto auditory = dnxr::AuditoryTonotopicEncoder{}.encode(audioFrame, {}, auditorySafety);
  const auto auditoryPacket = dnxr::AuditoryVectorToSpikeAdapter{}.adapt(88, 456.0, auditory);
  assert(auditoryPacket.modality == dnxr::SpikeModality::Auditory);
  assert(auditoryPacket.channelCount == 12U);

  dnxr::AvatarContactFrame contacts;
  contacts.pressure01 = 0.35f;
  contacts.area01 = 0.25f;
  contacts.flutter01 = 0.12f;
  contacts.texture01 = 0.22f;
  contacts.rightHand01 = 0.40f;
  contacts.requestsPain = true;
  const auto touch = dnxr::NeuralVirtualEpidermisEncoder{}.encode(contacts, {}, {});
  assert(touch.painBlocked01 == 1.0f);
  assert(touch.burnBlocked01 == 1.0f);
  assert(touch.crushBlocked01 == 1.0f);
  assert(touch.shockBlocked01 == 1.0f);
  assert(dnxr::TouchVectorToSpikeAdapter{}.adapt(88, 456.0, touch).channelCount == 18U);

  dnxr::AvatarPoseFrame pose;
  pose.head01 = 0.30f;
  pose.torso01 = 0.35f;
  pose.rightArm01 = 0.45f;
  pose.handConfidence01 = 0.90f;
  pose.footConfidence01 = 0.85f;
  pose.ownership01 = 0.95f;
  const auto proprioception = dnxr::NeuralProprioceptionEncoder{}.encode(pose);
  assert(dnxr::ProprioceptionVectorToSpikeAdapter{}.adapt(88, 456.0, proprioception).channelCount == 10U);

  dnxr::AvatarMotionFrame motion;
  motion.linearAcceleration01 = 0.20f;
  motion.angularVelocity01 = 0.18f;
  motion.gravityDirection01 = 0.50f;
  motion.balanceError01 = 0.08f;
  motion.sicknessRisk01 = 0.10f;
  motion.visualVestibularMismatch01 = 0.12f;
  const auto vestibular = dnxr::NeuralVestibularEncoder{}.encode(motion);
  assert(dnxr::VestibularVectorToSpikeAdapter{}.adapt(88, 456.0, vestibular).channelCount == 6U);

  dnxr::VirtualOdorField odor;
  odor.presence01 = 0.80f;
  dnxr::PatientOlfactoryMap odorMap;
  odorMap.experimentalChannelEnabled = false;
  const auto olfactory = dnxr::NeuralOlfactoryEncoder{}.encode(odor, odorMap, {});
  assert(olfactory.odorPresence01 == 0.0f);
  assert(dnxr::OlfactoryVectorToSpikeAdapter{}.adapt(88, 456.0, olfactory).chemicalLike);

  dnxr::VirtualTasteFrame taste;
  taste.sweet01 = 0.70f;
  dnxr::PatientGustatoryMap tasteMap;
  tasteMap.experimentalChannelEnabled = false;
  const auto gustatory = dnxr::NeuralGustatoryEncoder{}.encode(taste, tasteMap, {});
  assert(gustatory.sweet01 == 0.0f);
  assert(dnxr::GustatoryVectorToSpikeAdapter{}.adapt(88, 456.0, gustatory).chemicalLike);

  dnxr::VirtualSceneFrame scene;
  scene.luminance01 = 0.25f;
  scene.contrast01 = 0.20f;
  scene.edgeDensity01 = 0.15f;
  dnxr::PatientVisualMap visualMap;
  visualMap.retinotopicConfidence01 = 0.90f;
  const auto visual = dnxr::VisualCorticalWorldEncoder{}.encode(scene, visualMap, {});

  dnxr::StimulusBySpikeInput input;
  input.frameId = 88;
  input.timestampMs = 456.0;
  input.visual = visual;
  input.auditory = auditory;
  input.touch = touch;
  input.proprioception = proprioception;
  input.vestibular = vestibular;
  input.olfactory = olfactory;
  input.gustatory = gustatory;
  input.patientMap = enabledNonChemicalMap();
  input.realityAnchor = {};
  input.safety.simulationOnly = true;
  input.lastAck.packetAccepted = true;
  input.lastAck.signalQuality = 1.0f;
  input.sessionState.realityAnchorStability01 = 1.0f;

  const auto result = dnxr::StimulusBySpikeRuntime{}.process(input);
  assert(result.stageCoverageComplete);
  assert(result.pppGrammarFrame.modalityCount == dnxr::DNXR_SPIKE_BRIDGE_MODALITY_COUNT);
  assert(result.engineSpike.laneCount == dnxr::DNXR_SPIKE_BRIDGE_LANES);
  assert(result.engineSpike.modalityLoad01[static_cast<std::size_t>(dnxr::SpikeModality::RealityAnchor)] > 0.0f);
  assert(result.engineSpike.totalSpikeLoad01 <= dnxr::DNXR_NEURAL_CHIP_ACTIVE_CEILING_01);

  dnxr::FullDiveChipFrame frame = result.chipFrame;
  frame = dnxr::PatientModalityPolicyGate{}.apply(frame, input.patientMap);
  frame = dnxr::ExperimentalChemicalChannelGate{}.apply(frame);
  assert(frame.olfactory.odorPresence01 == 0.0f);
  assert(frame.gustatory.sweet01 == 0.0f);

  dnxr::ChipCapabilities caps;
  caps.canVisual = true;
  caps.canAuditory = true;
  caps.canHaptic = true;
  caps.canProprioceptive = true;
  caps.canVestibular = true;
  caps.canOlfactory = false;
  caps.canGustatory = false;
  caps.supportsHardwareCutoff = true;
  caps.supportsClosedLoopFeedback = true;
  caps.electrodeCount = dnxr::DNXR_SPIKE_BRIDGE_LANES;
  caps.maxStimulusChannels = dnxr::DNXR_SPIKE_BRIDGE_LANES;
  frame = dnxr::ChipCapabilityContract{caps}.apply(frame);
  assert(dnxr::DisabledModalityNeutralityValidator{}.validate(frame, caps).accepted);

  dnxr::NeuralChipLatencyBudget budget;
  budget.audioVisualSkewMs = 10.0f;
  budget.motorProprioceptionSkewMs = 8.0f;
  const auto phaseDecision = dnxr::MultisensoryPhaseLockOrchestrator{}.decide(frame, budget);
  assert(phaseDecision.allowFrame);
  frame = dnxr::MultisensoryPhaseLockOrchestrator{}.synchronize(frame, budget);
  frame = dnxr::ModalityExposureLedger{}.apply(frame, dnxr::PerModalityRateAndDosePolicy{}.policy());
  frame = dnxr::PerModalitySafetyEnvelope{}.apply(frame);
  assert(frame.realityAnchor.required);
  assert(frame.safety.rawSensoryPayloadBlocked);
  return 0;
}
