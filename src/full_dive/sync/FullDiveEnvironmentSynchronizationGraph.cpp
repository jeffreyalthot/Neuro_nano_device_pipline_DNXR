#include "full_dive/sync/FullDiveEnvironmentSynchronizationGraph.h"
#include "full_dive/spike_bridge/SpikeBridgeTypes.h"
#include <algorithm>
namespace dnxr {
EnvironmentSyncNode FullDiveEnvironmentSynchronizationGraph::buildNode(
    const NeuralChipOnlyWorldFrame &w) const {
  EnvironmentSyncNode n;
  n.frameId = w.frameId;
  n.timestampMs = w.timestampMs;
  n.visualCoherence01 = w.realityAnchor.required ? 1.0f : 0.0f;
  n.globalCoherence01 =
      std::min({n.visualCoherence01, n.auditoryCoherence01, n.touchCoherence01,
                n.motorCoherence01, n.vestibularCoherence01,
                n.olfactoryCoherence01, n.gustatoryCoherence01});
  return n;
}
float FullDiveEnvironmentSynchronizationGraph::compare(
    const EnvironmentSyncNode &a, const EnvironmentSyncNode &b) const {
  return spikeClamp01(1.0f - static_cast<float>(std::abs(static_cast<long long>(
                                 b.frameId - a.frameId))) *
                                 0.1f);
}
bool FullDiveEnvironmentSynchronizationGraph::requiresHold(
    const EnvironmentSyncNode &n) const {
  return n.globalCoherence01 < 0.25f;
}
const char *FullDiveEnvironmentSynchronizationGraph::holdReason(
    const EnvironmentSyncNode &n) const {
  return requiresHold(n) ? "environment_sync_coherence_hold" : "accepted";
}
} // namespace dnxr
