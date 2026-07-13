#include "full_dive/spike_bridge/PPPGrammarFrameBuilder.h"

#include <algorithm>
#include <cstdarg>
#include <cstdio>
#include <cstdint>
#include <cstring>

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

std::uint32_t hashCombine(std::uint32_t hash, std::uint32_t value) {
  hash ^= value;
  hash *= 16777619U;
  return hash;
}

std::uint32_t quantize01(float value) {
  return static_cast<std::uint32_t>(spikeClamp01(value) * 1000000.0f + 0.5f);
}

std::uint32_t integrityHashFor(const PPPGrammarFrame &frame) {
  std::uint32_t hash = 2166136261U;
  hash = hashCombine(hash, static_cast<std::uint32_t>(frame.frameId));
  hash = hashCombine(hash, static_cast<std::uint32_t>(frame.frameId >> 32U));
  hash = hashCombine(hash, static_cast<std::uint32_t>(frame.validFromFrame));
  hash = hashCombine(hash, static_cast<std::uint32_t>(frame.expiresAtFrame));
  hash = hashCombine(hash, quantize01(frame.globalSync01));
  hash = hashCombine(hash, quantize01(frame.activeCeiling01));
  hash = hashCombine(hash, frame.feedbackOnly ? 1U : 0U);
  hash = hashCombine(hash, frame.stimulusCandidateAllowed ? 1U : 0U);
  hash = hashCombine(hash, frame.ackReceived ? 1U : 0U);
  hash = hashCombine(hash, frame.emergencyLatched ? 1U : 0U);
  hash = hashCombine(hash, static_cast<std::uint32_t>(frame.hardwareElectrodeCount));
  hash = hashCombine(hash, static_cast<std::uint32_t>(frame.hardwareWireCount));
  hash = hashCombine(hash, static_cast<std::uint32_t>(frame.hardwareStimulusChannelLimit));
  hash = hashCombine(hash, static_cast<std::uint32_t>(frame.modalityCount));
  const auto count = std::min(frame.modalityCount, frame.modalities.size());
  for (std::size_t index = 0; index < count; ++index) {
    const auto &entry = frame.modalities[index];
    hash = hashCombine(hash, entry.modalityId);
    hash = hashCombine(hash, entry.channelId);
    hash = hashCombine(hash, quantize01(entry.channelConfidence01));
    hash = hashCombine(hash, quantize01(entry.maxActiveIntensity01));
    hash = hashCombine(hash, entry.stimulusUnlocked ? 1U : 0U);
    hash = hashCombine(hash, entry.electrodeStart);
    hash = hashCombine(hash, entry.electrodeCount);
    hash = hashCombine(hash, entry.wireId);
    hash = hashCombine(hash, quantize01(entry.phaseSlot01));
    hash = hashCombine(hash, quantize01(entry.risk01));
  }
  return hash == 0U ? 1U : hash;
}

void append(PPPGrammarSerializedFrame &out, const char *format, ...) {
  if (!out.accepted || out.size >= out.bytes.size()) {
    return;
  }
  va_list args;
  va_start(args, format);
  const int written = std::vsnprintf(out.bytes.data() + out.size,
                                     out.bytes.size() - out.size, format, args);
  va_end(args);
  if (written < 0 || static_cast<std::size_t>(written) >= out.bytes.size() - out.size) {
    out.accepted = false;
    out.reason = "ppp_serialized_capacity_exceeded";
    out.size = 0;
    out.bytes[0] = '\0';
    return;
  }
  out.size += static_cast<std::size_t>(written);
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
  frame.hardwareElectrodeCount = DNXR_SPIKE_BRIDGE_LANES;
  frame.hardwareWireCount = DNXR_SPIKE_BRIDGE_LANES / DNXR_SPIKE_BRIDGE_CHANNELS_PER_WIRE;
  frame.hardwareStimulusChannelLimit = DNXR_SPIKE_BRIDGE_LANES;
  frame.modalityCount = std::min(vectors.modalityCount, frame.modalities.size());

  std::uint16_t nextElectrode = 0;
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
    const std::uint16_t requested = static_cast<std::uint16_t>(
        std::min<std::size_t>(packet.channelCount, DNXR_SPIKE_BRIDGE_VECTOR_CHANNELS));
    const std::uint16_t remaining = nextElectrode < DNXR_SPIKE_BRIDGE_LANES
        ? static_cast<std::uint16_t>(DNXR_SPIKE_BRIDGE_LANES - nextElectrode)
        : 0U;
    entry.electrodeStart = nextElectrode;
    entry.electrodeCount = std::min(requested, remaining);
    entry.wireId = static_cast<std::uint16_t>(
        entry.electrodeStart / DNXR_SPIKE_BRIDGE_CHANNELS_PER_WIRE);
    entry.phaseSlot01 = spikeClamp01(packet.phaseSlot01);
    entry.risk01 = spikeClamp01(packet.risk01);
    nextElectrode = static_cast<std::uint16_t>(nextElectrode + entry.electrodeCount);
  }
  frame.integrityHash = integrityHashFor(frame);
  return frame;
}

PPPGrammarSerializedFrame
PPPGrammarFrameBuilder::serialize(const PPPGrammarFrame &frame) const {
  PPPGrammarSerializedFrame out;
  out.accepted = verify(frame);
  out.reason = out.accepted ? "accepted" : "ppp_frame_verification_failed";
  if (!out.accepted) {
    return out;
  }
  append(out,
         "PPP|%s|%s|%llu|%llu|%llu|%zu|%.6f|%.6f|%u|%u|%u|%u|%zu|%zu|%zu|%zu|%u\n",
         frame.pppVersion, frame.grammarId,
         static_cast<unsigned long long>(frame.frameId),
         static_cast<unsigned long long>(frame.validFromFrame),
         static_cast<unsigned long long>(frame.expiresAtFrame),
         frame.grammarRefreshFrames, frame.globalSync01, frame.activeCeiling01,
         frame.feedbackOnly ? 1U : 0U,
         frame.stimulusCandidateAllowed ? 1U : 0U, frame.ackRequired ? 1U : 0U,
         frame.ackReceived ? 1U : 0U, frame.hardwareElectrodeCount,
         frame.hardwareWireCount, frame.hardwareStimulusChannelLimit,
         frame.modalityCount, frame.integrityHash);
  for (std::size_t index = 0; index < frame.modalityCount; ++index) {
    const auto &entry = frame.modalities[index];
    append(out, "M|%zu|%u|%u|%.6f|%.6f|%u|%u|%u|%.6f|%.6f|%u\n", index,
           entry.modalityId, entry.channelId, entry.channelConfidence01,
           entry.maxActiveIntensity01, entry.stimulusUnlocked ? 1U : 0U,
           entry.electrodeStart, entry.electrodeCount, entry.phaseSlot01,
           entry.risk01, entry.wireId);
  }
  out.integrityHash = frame.integrityHash;
  return out;
}

PPPGrammarFrame
PPPGrammarFrameBuilder::parse(const PPPGrammarSerializedFrame &serialized) const {
  PPPGrammarFrame frame;
  if (!serialized.accepted || serialized.size == 0U) {
    frame.integrityHash = 0U;
    return frame;
  }
  const char *cursor = serialized.bytes.data();
  unsigned feedbackOnly = 0U;
  unsigned stimulusAllowed = 0U;
  unsigned ackRequired = 0U;
  unsigned ackReceived = 0U;
  unsigned parsedHash = 0U;
  char version[16]{};
  char grammar[48]{};
  unsigned long long frameId = 0ULL;
  unsigned long long validFrom = 0ULL;
  unsigned long long expiresAt = 0ULL;
  int consumed = 0;
  const int headerFields = std::sscanf(
      cursor,
      "PPP|%15[^|]|%47[^|]|%llu|%llu|%llu|%zu|%f|%f|%u|%u|%u|%u|%zu|%zu|%zu|%zu|%u\n%n",
      version, grammar, &frameId, &validFrom, &expiresAt,
      &frame.grammarRefreshFrames, &frame.globalSync01, &frame.activeCeiling01,
      &feedbackOnly, &stimulusAllowed, &ackRequired, &ackReceived,
      &frame.hardwareElectrodeCount, &frame.hardwareWireCount,
      &frame.hardwareStimulusChannelLimit, &frame.modalityCount, &parsedHash,
      &consumed);
  if (headerFields != 17 || consumed <= 0) {
    frame.integrityHash = 0U;
    return frame;
  }
  frame.frameId = static_cast<std::uint64_t>(frameId);
  frame.validFromFrame = static_cast<std::uint64_t>(validFrom);
  frame.expiresAtFrame = static_cast<std::uint64_t>(expiresAt);
  frame.feedbackOnly = feedbackOnly != 0U;
  frame.stimulusCandidateAllowed = stimulusAllowed != 0U;
  frame.ackRequired = ackRequired != 0U;
  frame.ackReceived = ackReceived != 0U;
  frame.modalityCount = std::min(frame.modalityCount, frame.modalities.size());
  cursor += consumed;
  for (std::size_t line = 0; line < frame.modalityCount; ++line) {
    std::size_t index = 0;
    unsigned modality = 0U;
    unsigned channel = 0U;
    unsigned unlocked = 0U;
    unsigned electrodeStart = 0U;
    unsigned electrodeCount = 0U;
    unsigned wireId = 0U;
    consumed = 0;
    auto &entry = frame.modalities[line];
    const int fields = std::sscanf(
        cursor, "M|%zu|%u|%u|%f|%f|%u|%u|%u|%f|%f|%u\n%n", &index,
        &modality, &channel, &entry.channelConfidence01,
        &entry.maxActiveIntensity01, &unlocked, &electrodeStart,
        &electrodeCount, &entry.phaseSlot01, &entry.risk01, &wireId,
        &consumed);
    if (fields != 11 || consumed <= 0 || index != line) {
      frame.integrityHash = 0U;
      return frame;
    }
    entry.modalityId = static_cast<std::uint16_t>(modality);
    entry.channelId = static_cast<std::uint16_t>(channel);
    entry.stimulusUnlocked = unlocked != 0U;
    entry.electrodeStart = static_cast<std::uint16_t>(electrodeStart);
    entry.electrodeCount = static_cast<std::uint16_t>(electrodeCount);
    entry.wireId = static_cast<std::uint16_t>(wireId);
    cursor += consumed;
  }
  frame.integrityHash = parsedHash;
  return frame;
}

bool PPPGrammarFrameBuilder::verify(const PPPGrammarFrame &frame) const {
  if (frame.integrityHash == 0U || frame.integrityHash != integrityHashFor(frame)) {
    return false;
  }
  if (frame.modalityCount > frame.modalities.size() || frame.hardwareElectrodeCount == 0U ||
      frame.hardwareStimulusChannelLimit == 0U || frame.hardwareWireCount == 0U) {
    return false;
  }
  if (frame.validFromFrame > frame.frameId || frame.expiresAtFrame <= frame.validFromFrame) {
    return false;
  }
  const auto count = std::min(frame.modalityCount, frame.modalities.size());
  for (std::size_t index = 0; index < count; ++index) {
    const auto &entry = frame.modalities[index];
    const std::size_t end = static_cast<std::size_t>(entry.electrodeStart) +
                            static_cast<std::size_t>(entry.electrodeCount);
    if (end > frame.hardwareStimulusChannelLimit || end > frame.hardwareElectrodeCount) {
      return false;
    }
    if (entry.electrodeCount > 0U && entry.wireId >= frame.hardwareWireCount) {
      return false;
    }
    if (entry.maxActiveIntensity01 > frame.activeCeiling01 || entry.channelConfidence01 < 0.0f ||
        entry.channelConfidence01 > 1.0f || entry.risk01 < 0.0f || entry.risk01 > 1.0f) {
      return false;
    }
  }
  return true;
}

} // namespace dnxr
