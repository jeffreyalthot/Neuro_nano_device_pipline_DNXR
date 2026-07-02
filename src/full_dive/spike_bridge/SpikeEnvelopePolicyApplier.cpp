#include "full_dive/spike_bridge/SpikeEnvelopePolicyApplier.h"
namespace dnxr {
EngineSpikeFrame
SpikeEnvelopePolicyApplier::apply(const EngineSpikeFrame &input,
                                  const NeuralChipSessionState &s) const {
  auto o = input;
  float scale = spikeClamp01(1.0f - (s.sessionFatigue01 * 0.5f) -
                             (s.sessionRisk01 * 0.25f));
  for (size_t i = 0; i < o.laneCount; i++)
    o.spikeLanes01[i] *= scale;
  if (s.realityAnchorStability01 < 0.3f) {
    o.holdRequired = true;
    o.accepted = false;
    o.reason = "reality_anchor_weak_spike_hold";
  }
  sanitize(o);
  return o;
}
} // namespace dnxr
