#include "full_dive/spike_bridge/StimulusBySpikePacketAssembler.h"
#include <algorithm>
namespace dnxr {
FullDiveChipFrame StimulusBySpikePacketAssembler::assemble(
    const StimulusBySpikeFrame &s, const NeuralVisualVector &v,
    const NeuralAuditoryVector &a, const NeuralTouchVector &t,
    const NeuralProprioceptionVector &p, const NeuralVestibularVector &ve,
    const NeuralMotorIntentVector &m, const NeuralOlfactoryVector &o,
    const NeuralGustatoryVector &g, const NeuralRealityAnchorVector &r,
    const NeuralSafetyVector &safety) const {
  FullDiveChipFrame f;
  f.frameId = s.frameId;
  f.timestampMs = s.timestampMs;
  f.visual = v;
  f.auditory = a;
  f.touch = t;
  f.proprioception = p;
  f.vestibular = ve;
  f.motorIntent = m;
  f.olfactory = o;
  f.gustatory = g;
  f.realityAnchor = r;
  f.safety = safety;
  f.safety.aggregateRisk01 =
      spikeClamp01(std::max(f.safety.aggregateRisk01, s.riskScore01));
  f.safety.activeCeiling01 =
      spikeClamp01(std::min(f.safety.activeCeiling01, s.intensityScale01));
  return f;
}
} // namespace dnxr
