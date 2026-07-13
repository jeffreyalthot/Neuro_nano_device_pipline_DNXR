#include "full_dive/spike_bridge/PPPGrammarFrameBuilder.h"
#include "full_dive/spike_bridge/StimulusBySpikeRuntime.h"
#include "full_dive/spike_bridge/VectorIntentNormalizer.h"

#include <cassert>
#include <cstring>

int main() {
  dnxr::NeuralVisualVector visual;
  visual.globalLuminance01 = 0.25f;
  visual.contrast01 = 0.20f;
  visual.depthNear01 = 0.35f;
  visual.phospheneDensity01 = 0.30f;
  visual.phospheneStability01 = 0.80f;

  dnxr::NeuralAuditoryVector auditory;
  auditory.loudness01 = 0.30f;
  auditory.midBand01 = 0.20f;
  auditory.speechPriority01 = 0.30f;

  dnxr::NeuralTouchVector touch;
  touch.contactPressure01 = 0.20f;
  touch.painBlocked01 = 1.0f;
  touch.burnBlocked01 = 1.0f;
  touch.crushBlocked01 = 1.0f;
  touch.shockBlocked01 = 1.0f;
  touch.somatosensoryConfidence01 = 0.90f;

  dnxr::NeuralRealityAnchorVector anchor;
  dnxr::VectorBundleFrame vectors = dnxr::VectorIntentNormalizer{}.normalize(
      901, 12.0, visual, auditory, touch, {}, {}, {}, {}, {}, anchor);
  vectors.globalCoherence01 = 0.95f;
  vectors.sceneContinuity01 = 0.95f;
  vectors.bodyOwnership01 = 0.95f;

  dnxr::NeuralChipSessionState session;
  session.realityAnchorStability01 = 0.95f;
  dnxr::NeuralChipAck ack;
  ack.packetAccepted = true;
  ack.signalQuality = 0.95f;

  dnxr::PPPGrammarFrameBuilder builder;
  const auto grammar = builder.build(vectors, session, ack);
  assert(builder.verify(grammar));
  assert(grammar.integrityHash != 0U);
  assert(grammar.hardwareElectrodeCount == dnxr::DNXR_SPIKE_BRIDGE_LANES);
  assert(grammar.hardwareWireCount == dnxr::DNXR_SPIKE_BRIDGE_LANES / dnxr::DNXR_SPIKE_BRIDGE_CHANNELS_PER_WIRE);
  assert(grammar.hardwareStimulusChannelLimit == dnxr::DNXR_SPIKE_BRIDGE_LANES);
  assert(grammar.modalities[0].electrodeStart == 0U);
  for (std::size_t i = 0; i < grammar.modalityCount; ++i) {
    const auto &entry = grammar.modalities[i];
    assert(static_cast<std::size_t>(entry.electrodeStart) + entry.electrodeCount <=
           grammar.hardwareStimulusChannelLimit);
    assert(entry.wireId < grammar.hardwareWireCount || entry.electrodeCount == 0U);
  }

  const auto serialized = builder.serialize(grammar);
  assert(serialized.accepted);
  assert(serialized.size > 0U);
  assert(std::strstr(serialized.bytes.data(), "PPP|0.7.0|DNXR_SPIKE_GRAMMAR_MAIN|901") != nullptr);
  assert(serialized.integrityHash == grammar.integrityHash);

  const auto parsed = builder.parse(serialized);
  assert(parsed.frameId == grammar.frameId);
  assert(parsed.validFromFrame == grammar.validFromFrame);
  assert(parsed.expiresAtFrame == grammar.expiresAtFrame);
  assert(parsed.modalityCount == grammar.modalityCount);
  assert(parsed.integrityHash == grammar.integrityHash);
  assert(builder.verify(parsed));

  auto tampered = grammar;
  tampered.modalities[0].electrodeStart = static_cast<std::uint16_t>(dnxr::DNXR_SPIKE_BRIDGE_LANES);
  tampered.modalities[0].electrodeCount = 2U;
  assert(!builder.verify(tampered));

  dnxr::NeuralChipAck lowAck;
  lowAck.packetAccepted = false;
  lowAck.signalQuality = 0.0f;
  const auto feedbackOnly = builder.build(vectors, session, lowAck);
  assert(feedbackOnly.feedbackOnly);
  assert(!feedbackOnly.stimulusCandidateAllowed);
  assert(builder.verify(feedbackOnly));

  dnxr::StimulusBySpikeInput weakFeedbackInput;
  weakFeedbackInput.frameId = 902;
  weakFeedbackInput.visual = visual;
  weakFeedbackInput.patientMap = dnxr::PatientSpecificNeuralMap::simulationDefaults();
  weakFeedbackInput.patientMap.visual.enabled = true;
  weakFeedbackInput.patientMap.visual.confidence01 = 0.95f;
  weakFeedbackInput.patientMap.visual.maturity = dnxr::NeuralChannelMaturity::SupportedSimulation;
  weakFeedbackInput.safety.simulationOnly = true;
  weakFeedbackInput.lastAck.packetAccepted = true;
  weakFeedbackInput.lastAck.signalQuality = 0.50f;
  weakFeedbackInput.readFrame.signalQuality01 = 1.0f;
  weakFeedbackInput.sessionState.realityAnchorStability01 = 1.0f;
  const auto weakRuntime = dnxr::StimulusBySpikeRuntime{}.process(weakFeedbackInput);
  assert(weakRuntime.pppGrammarFrame.feedbackOnly);
  assert(!weakRuntime.accepted);
  assert(weakRuntime.holdRequired);
  assert(std::strcmp(weakRuntime.reason, "ppp_feedback_only_lifecycle_hold") == 0);
  return 0;
}
