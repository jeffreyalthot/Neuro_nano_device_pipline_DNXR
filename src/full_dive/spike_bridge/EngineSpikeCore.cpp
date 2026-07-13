#include "full_dive/spike_bridge/EngineSpikeCore.h"

#include <array>
#include <cstddef>

namespace dnxr {

EngineSpikeFrame
EngineSpikeCore::process(const SpikeEngineIntentFrame &intent) const {
  EngineSpikeFrame output;
  output.frameId = intent.frameId;
  output.timestampMs = intent.timestampMs;
  output.holdRequired = intent.holdRequired;
  output.accepted = !intent.holdRequired;
  output.reason = intent.holdRequired ? intent.holdReason : "accepted";
  output.phaseLockQuality01 = intent.phaseLockQuality01;
  output.predictionConfidence01 = intent.expectedFeedbackQuality01;
  output.feedbackNeed01 = 1.0f - intent.expectedFeedbackQuality01;
  output.safetyRisk01 =
      spikeClamp01((intent.vestibularRisk01 + intent.chemicalRisk01 +
                    (1.0f - intent.phaseLockQuality01)) /
                   3.0f);

  constexpr std::array<SpikeModality, DNXR_SPIKE_BRIDGE_MODALITY_COUNT>
      priorityOrder = {SpikeModality::RealityAnchor, SpikeModality::Motor,
                       SpikeModality::Touch, SpikeModality::Proprioception,
                       SpikeModality::Visual, SpikeModality::Auditory,
                       SpikeModality::Vestibular, SpikeModality::Olfactory,
                       SpikeModality::Gustatory};

  std::size_t laneIndex = 0;
  float totalLoad = 0.0f;
  for (const SpikeModality priorityModality : priorityOrder) {
    for (std::size_t modalityIndex = 0;
         modalityIndex < intent.routedCount &&
         modalityIndex < intent.routedModalities.size();
         ++modalityIndex) {
      const auto &packet = intent.routedModalities[modalityIndex];
      if (packet.modality != priorityModality) {
        continue;
      }
      float modalityLoad = 0.0f;
      std::size_t emittedChannels = 0;
      for (std::size_t channelIndex = 0; channelIndex < packet.channelCount &&
                                         laneIndex < output.spikeLanes01.size();
           ++channelIndex) {
        const float lane =
            spikeClamp01(packet.channels01[channelIndex] * packet.confidence01 *
                         (1.0f - packet.risk01));
        output.spikeLanes01[laneIndex++] = lane;
        modalityLoad += lane;
        totalLoad += lane;
        ++emittedChannels;
      }
      const auto loadIndex = static_cast<std::size_t>(packet.modality);
      if (loadIndex < output.modalityLoad01.size()) {
        output.modalityLoad01[loadIndex] =
            emittedChannels > 0 ? spikeClamp01(modalityLoad / emittedChannels)
                                : 0.0f;
      }
    }
  }

  output.laneCount = laneIndex;
  output.totalSpikeLoad01 =
      laneIndex > 0 ? spikeClamp01(totalLoad / laneIndex) : 0.0f;
  sanitize(output);
  return output;
}

} // namespace dnxr
