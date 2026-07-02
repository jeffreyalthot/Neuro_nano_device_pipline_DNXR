#include "full_dive/spike_bridge/EngineSpikeOutputValidator.h"

#include <cmath>

namespace dnxr {
namespace {
bool finite01(float value) {
  return std::isfinite(value) && value >= 0.0f && value <= 1.0f;
}
} // namespace

EngineSpikeFrame
EngineSpikeOutputValidator::validate(const EngineSpikeFrame &input) const {
  auto output = input;
  if (!output.accepted) {
    sanitize(output);
    return output;
  }

  if (output.laneCount > output.spikeLanes01.size()) {
    output.accepted = false;
    output.reason = "spike_lane_count_exceeds_fixed_capacity";
  }

  for (float lane : output.spikeLanes01) {
    if (!std::isfinite(lane)) {
      output.accepted = false;
      output.reason = "non_finite_spike_lane";
      break;
    }
    if (lane < 0.0f || lane > 1.0f) {
      output.accepted = false;
      output.reason = "spike_lane_out_of_range";
      break;
    }
  }

  if (!finite01(output.totalSpikeLoad01)) {
    output.accepted = false;
    output.reason = "total_spike_load_out_of_range";
  }
  if (!finite01(output.phaseLockQuality01)) {
    output.accepted = false;
    output.reason = "phase_lock_quality_out_of_range";
  }
  if (!finite01(output.predictionConfidence01)) {
    output.accepted = false;
    output.reason = "prediction_confidence_out_of_range";
  }
  if (!finite01(output.feedbackNeed01)) {
    output.accepted = false;
    output.reason = "feedback_need_out_of_range";
  }
  if (!finite01(output.safetyRisk01)) {
    output.accepted = false;
    output.reason = "spike_safety_risk_out_of_range";
  }

  sanitize(output);
  return output;
}
} // namespace dnxr
