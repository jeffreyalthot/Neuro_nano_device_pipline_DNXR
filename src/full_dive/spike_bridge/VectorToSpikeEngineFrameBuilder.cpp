#include "full_dive/spike_bridge/VectorToSpikeEngineFrameBuilder.h"
#include <algorithm>
namespace dnxr {
SpikeEngineIntentFrame
VectorToSpikeEngineFrameBuilder::build(const VectorBundleFrame &in) const {
  SpikeEngineIntentFrame o;
  o.frameId = in.frameId;
  o.timestampMs = in.timestampMs;
  o.routedModalities = in.modalities;
  o.routedCount = in.modalityCount;
  o.phaseLockQuality01 = in.globalCoherence01;
  o.expectedFeedbackQuality01 = 1.0f - in.aggregateRisk01;
  float load = 0;
  for (const auto &p : in.modalities) {
    float pl = 0;
    for (size_t i = 0; i < p.channelCount; i++)
      pl += p.channels01[i];
    pl = p.channelCount ? pl / p.channelCount : 0;
    load += pl;
    if (p.motorLike)
      o.motorPriority01 = std::max(
          o.motorPriority01, std::max(p.priority01, 1.0f - p.confidence01));
    if (p.sensoryLike)
      o.sensoryPriority01 = std::max(o.sensoryPriority01, p.priority01);
    if (p.modality == SpikeModality::Vestibular)
      o.vestibularRisk01 = p.risk01;
    if (p.chemicalLike)
      o.chemicalRisk01 = std::max(o.chemicalRisk01, p.risk01);
  }
  o.globalLoad01 = spikeClamp01(load / 9.0f);
  if (!in.accepted) {
    o.holdRequired = true;
    o.holdReason = in.reason ? in.reason : "spike_intent_input_rejected_hold";
  } else if (in.globalCoherence01 < 0.25f) {
    o.holdRequired = true;
    o.holdReason = "spike_intent_low_global_coherence_hold";
  } else if (in.aggregateRisk01 > 0.90f) {
    o.holdRequired = true;
    o.holdReason = "spike_intent_aggregate_risk_hold";
  } else {
    o.holdRequired = false;
    o.holdReason = "none";
  }
  return o;
}
} // namespace dnxr
