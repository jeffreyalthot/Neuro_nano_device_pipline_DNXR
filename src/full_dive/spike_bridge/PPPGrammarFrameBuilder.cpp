#include "full_dive/spike_bridge/PPPGrammarFrameBuilder.h"

#include <algorithm>
#include <cstdint>

namespace dnxr {
namespace {
std::uint16_t modalityId(SpikeModality modality) {
  switch (modality) {
  case SpikeModality::Visual:
    return 0x01;
  case SpikeModality::Auditory:
    return 0x02;
  case SpikeModality::Touch:
    return 0x03;
  case SpikeModality::Proprioception:
    return 0x04;
  case SpikeModality::Vestibular:
    return 0x05;
  case SpikeModality::Motor:
    return 0x06;
  case SpikeModality::Olfactory:
    return 0x07;
  case SpikeModality::Gustatory:
    return 0x08;
  case SpikeModality::RealityAnchor:
    return 0xF0;
  }
  return 0xF0;
}

const char *modalityName(SpikeModality modality) {
  switch (modality) {
  case SpikeModality::Visual:
    return "VISION";
  case SpikeModality::Auditory:
    return "AUDIO";
  case SpikeModality::Touch:
    return "TACTILE";
  case SpikeModality::Proprioception:
    return "PROPRIOCEPTION";
  case SpikeModality::Vestibular:
    return "VESTIBULAR";
  case SpikeModality::Motor:
    return "MOTOR";
  case SpikeModality::Olfactory:
    return "OLFACTORY";
  case SpikeModality::Gustatory:
    return "GUSTATORY";
  case SpikeModality::RealityAnchor:
    return "SAFETY";
  }
  return "SAFETY";
}

const char *primaryChannel(SpikeModality modality) {
  switch (modality) {
  case SpikeModality::Visual:
    return "vision.depth";
  case SpikeModality::Auditory:
    return "audio.volume";
  case SpikeModality::Touch:
    return "tactile.pressure";
  case SpikeModality::Proprioception:
    return "proprio.body_pose";
  case SpikeModality::Vestibular:
    return "vestibular.balance";
  case SpikeModality::Motor:
    return "motor.intent_vector";
  case SpikeModality::Olfactory:
    return "olfactory.presence";
  case SpikeModality::Gustatory:
    return "gustatory.taste";
  case SpikeModality::RealityAnchor:
    return "safety.reality_anchor";
  }
  return "safety.reality_anchor";
}
} // namespace

PPPGrammarFrame PPPGrammarFrameBuilder::build(
    const VectorBundleFrame &vectors, const NeuralChipSessionState &session,
    const NeuralChipAck &ack) const {
  PPPGrammarFrame frame;
  frame.frameId = vectors.frameId;
  frame.validFromFrame = vectors.frameId;
  frame.expiresAtFrame = vectors.frameId + DNXR_PPP_GRAMMAR_REFRESH_FRAMES;
  frame.grammarRefreshFrames = DNXR_PPP_GRAMMAR_REFRESH_FRAMES;
  frame.globalSync01 = spikeClamp01(std::min({vectors.globalCoherence01,
                                              vectors.sceneContinuity01,
                                              vectors.bodyOwnership01,
                                              session.realityAnchorStability01,
                                              ack.signalQuality}));
  frame.clinicalGated = true;
  frame.feedbackOnly = frame.globalSync01 < DNXR_PPP_STIMULUS_SYNC_THRESHOLD_01;
  frame.ackRequired = true;
  frame.ackReceived = ack.packetAccepted;
  frame.emergencyLatched = session.emergencyStopLatched;
  frame.stimulusCandidateAllowed = !frame.feedbackOnly && frame.ackReceived &&
                                    !frame.emergencyLatched;
  frame.activeCeiling01 = DNXR_NEURAL_CHIP_ACTIVE_CEILING_01;
  frame.blockedDangerSymbols = true;
  frame.rawSensoryPayloadBlocked = true;
  frame.modalityCount = std::min(vectors.modalityCount, frame.modalities.size());

  for (std::size_t index = 0; index < frame.modalityCount; ++index) {
    const auto &packet = vectors.modalities[index];
    auto &entry = frame.modalities[index];
    entry.modalityId = modalityId(packet.modality);
    entry.channelId = static_cast<std::uint16_t>((entry.modalityId << 8) |
                                                 ((index + 1U) & 0x00FFU));
    entry.modalityName = modalityName(packet.modality);
    entry.semanticPath = primaryChannel(packet.modality);
    entry.channelConfidence01 = spikeClamp01(packet.confidence01);
    entry.maxActiveIntensity01 = DNXR_NEURAL_CHIP_ACTIVE_CEILING_01;
    entry.requiresAck = true;
    entry.requiresClinicalGate = true;
    entry.feedbackExpected = true;
    entry.stimulusUnlocked = frame.stimulusCandidateAllowed && packet.enabled &&
                             packet.confidence01 >=
                                 DNXR_PPP_STIMULUS_SYNC_THRESHOLD_01 &&
                             packet.risk01 < 0.80f;
  }

  return frame;
}

} // namespace dnxr
