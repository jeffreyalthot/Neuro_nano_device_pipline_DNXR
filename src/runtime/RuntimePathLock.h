#pragma once

#include "core/RuntimeDeploymentMode.h"
#include "hardware/NeuralChipHAL.h"

namespace dnxr {

enum class RuntimePathKind {
  CanonicalAdapter,
  RealHardware,
  HILTestAdapter,
  DeterministicReplayAdapter
};

struct RuntimePathDecision {
  RuntimePathKind path = RuntimePathKind::CanonicalAdapter;
  bool lockedRealHardware = false;
  bool adapterFallbackAttempted = false;
  const char *reason = "canonical_environment_adapter";
};

class BackendAdapterLock {
public:
  RuntimePathDecision decide(const NeuralChipStatus &status,
                             RuntimeDeploymentMode deploymentMode);
  RuntimePathDecision current() const;
  bool realHardwareLocked() const;

private:
  RuntimePathDecision decision_{};
};

using RuntimePathLock [[deprecated("Use BackendAdapterLock; compatibility alias will be removed after canonical pipeline migration")]] = BackendAdapterLock;

} // namespace dnxr
