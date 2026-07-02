#include "full_dive/spike_bridge/SpikeToStimulusIntentTranslator.h"

#include <cstddef>

namespace dnxr {
namespace {
constexpr float kExposureRiskLimit01 = 0.80f;
}

StimulusBySpikeFrame SpikeToStimulusIntentTranslator::translate(
    const EngineSpikeFrame &frame) const {
  StimulusBySpikeFrame output;
  output.frameId = frame.frameId;
  output.timestampMs = frame.timestampMs;
  output.channelCount = frame.laneCount;
  output.accepted = frame.accepted && !frame.holdRequired;
  output.reason = output.accepted ? "accepted" : frame.reason;

  for (std::size_t index = 0;
       index < frame.laneCount && index < output.boundedChannels01.size();
       ++index) {
    output.boundedChannels01[index] = frame.spikeLanes01[index];
  }

  output.modalityIntensity01 = frame.modalityLoad01;
  output.aggregateSensoryLoad01 = frame.totalSpikeLoad01;
  output.intensityScale01 = spikeClamp01(1.0f - frame.safetyRisk01);
  output.riskScore01 = frame.safetyRisk01;
  output.feedbackExpectation01 = frame.predictionConfidence01;
  output.fiveSenseCoverage =
      frame.laneCount >= DNXR_SPIKE_BRIDGE_MODALITY_COUNT;
  output.allowDispatch = false;
  output.exposureBudgetOk = frame.safetyRisk01 < kExposureRiskLimit01;

  sanitize(output);
  return output;
}

} // namespace dnxr
