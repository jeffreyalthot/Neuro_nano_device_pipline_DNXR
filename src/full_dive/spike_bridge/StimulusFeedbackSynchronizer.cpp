#include "full_dive/spike_bridge/StimulusFeedbackSynchronizer.h"

namespace dnxr {
namespace {
constexpr float kMinimumFeedbackQualityForDispatch = 0.20f;
constexpr float kFeedbackHoldIntensityScale = 0.50f;
} // namespace

StimulusBySpikeFrame StimulusFeedbackSynchronizer::synchronizeFeedback(
    const StimulusBySpikeFrame &input, const ChipReadFrame &read,
    const NeuralChipAck &ack) {
  StimulusBySpikeFrame output = input;
  const float ackQuality01 =
      ack.packetAccepted ? spikeClamp01(ack.signalQuality) : 0.0f;
  quality_ = spikeClamp01((read.signalQuality01 + ackQuality01) * 0.5f);
  hold_ = !ack.packetAccepted || quality_ < kMinimumFeedbackQualityForDispatch;

  if (hold_) {
    output.accepted = false;
    output.allowDispatch = false;
    output.reason = "stimulus_feedback_ack_or_quality_hold";
    output.intensityScale01 *= kFeedbackHoldIntensityScale;
    output.riskScore01 =
        spikeClamp01(output.riskScore01 + (1.0f - quality_) * 0.25f);
  }

  output.feedbackExpectation01 = quality_;
  sanitize(output);
  return output;
}

float StimulusFeedbackSynchronizer::feedbackQuality01() const {
  return quality_;
}

bool StimulusFeedbackSynchronizer::requiresHold() const { return hold_; }

} // namespace dnxr
