#include "full_dive/spike_bridge/MultiLayerSpikeThinkingEngine.h"

#include <algorithm>
#include <cstddef>

namespace dnxr {
namespace {
float averageActiveChannels(const ModalityVectorPacket &packet) {
  if (packet.channelCount == 0) {
    return 0.0f;
  }
  float sum = 0.0f;
  for (std::size_t index = 0;
       index < packet.channelCount && index < packet.channels01.size();
       ++index) {
    sum += packet.channels01[index];
  }
  return spikeClamp01(sum / static_cast<float>(packet.channelCount));
}

float grammarUnlockFor(const PPPGrammarFrame &grammar, std::size_t index) {
  if (index >= grammar.modalityCount || index >= grammar.modalities.size()) {
    return 0.0f;
  }
  const auto &entry = grammar.modalities[index];
  return entry.stimulusUnlocked ? entry.channelConfidence01 : 0.0f;
}
} // namespace

SpikeEngineIntentFrame MultiLayerSpikeThinkingEngine::think(
    const SpikeEngineIntentFrame &intent, const PPPGrammarFrame &grammar,
    MultiThinkingFrame *thinkingFrame) const {
  auto output = intent;
  MultiThinkingFrame local;
  local.frameId = intent.frameId;
  local.timestampMs = intent.timestampMs;
  local.thinkingGroupCount =
      std::min(intent.routedCount, local.thinkingGroups.size());

  float coherenceSum = 0.0f;
  float confidenceSum = 0.0f;
  float safetySum = 0.0f;

  for (std::size_t index = 0; index < local.thinkingGroupCount; ++index) {
    auto &packet = output.routedModalities[index];
    const float featureLayer = averageActiveChannels(packet);
    const float temporalLayer = spikeClamp01(
        1.0f - std::min(packet.latencyBudgetMs, 32.0f) / 32.0f +
        packet.phaseSlot01 * 0.5f);
    const float safetyLayer = spikeClamp01(
        (1.0f - packet.risk01) * grammar.activeCeiling01 *
        (grammar.blockedDangerSymbols ? 1.0f : 0.0f));
    const float grammarConfidence = grammarUnlockFor(grammar, index);
    const float internalCoherence = spikeClamp01(
        (packet.confidence01 + packet.patientMapConfidence01 +
         grammarConfidence + safetyLayer) /
        4.0f);
    const float idealSpikeLayer = spikeClamp01(
        featureLayer * internalCoherence * safetyLayer);

    auto &summary = local.thinkingGroups[index];
    summary.modality = packet.modality;
    summary.featureLayer01 = featureLayer;
    summary.temporalLayer01 = temporalLayer;
    summary.safetyLayer01 = safetyLayer;
    summary.idealSpikeLayer01 = idealSpikeLayer;
    summary.internalSpikeCoherence01 = internalCoherence;

    packet.confidence01 = spikeClamp01(
        (packet.confidence01 * 0.50f) + (internalCoherence * 0.35f) +
        (grammarConfidence * 0.15f));
    packet.priority01 = spikeClamp01(
        std::max(packet.priority01, idealSpikeLayer));
    for (std::size_t channel = 0;
         channel < packet.channelCount && channel < packet.channels01.size();
         ++channel) {
      packet.channels01[channel] = spikeClamp01(
          packet.channels01[channel] * grammar.activeCeiling01 *
          internalCoherence);
    }
    sanitize(packet);

    coherenceSum += internalCoherence;
    confidenceSum += packet.confidence01;
    safetySum += safetyLayer;
  }

  if (local.thinkingGroupCount > 0) {
    const float divisor = static_cast<float>(local.thinkingGroupCount);
    local.crossGroupCoherence01 = spikeClamp01(coherenceSum / divisor);
    local.candidateConfidence01 = spikeClamp01(confidenceSum / divisor);
    local.safetyConsensus01 = spikeClamp01(safetySum / divisor);
  }

  output.phaseLockQuality01 = spikeClamp01(
      std::min(output.phaseLockQuality01, local.crossGroupCoherence01));
  output.expectedFeedbackQuality01 = spikeClamp01(
      (output.expectedFeedbackQuality01 * 0.60f) +
      (local.candidateConfidence01 * 0.40f));

  if (grammar.feedbackOnly || !grammar.stimulusCandidateAllowed) {
    local.accepted = true;
    local.reason = "multi_thinking_feedback_only_candidates";
  } else if (local.safetyConsensus01 < 0.20f) {
    local.accepted = false;
    local.reason = "multi_thinking_safety_consensus_hold";
    output.holdRequired = true;
    output.holdReason = local.reason;
  } else if (local.crossGroupCoherence01 < 0.20f) {
    local.accepted = false;
    local.reason = "multi_thinking_cross_group_coherence_hold";
    output.holdRequired = true;
    output.holdReason = local.reason;
  } else {
    local.accepted = true;
    local.reason = "multi_thinking_spike_candidates_ready";
  }

  if (thinkingFrame) {
    *thinkingFrame = local;
  }
  return output;
}

} // namespace dnxr
