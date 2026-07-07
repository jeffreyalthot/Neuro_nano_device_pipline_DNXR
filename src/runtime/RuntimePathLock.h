#pragma once

#include "core/RuntimeDeploymentMode.h"
#include "hardware/NeuralChipHAL.h"

namespace dnxr {

enum class RuntimePathKind {
  SecondarySimulation,
  PrimaryReal
};

struct RuntimePathDecision {
  RuntimePathKind path = RuntimePathKind::SecondarySimulation;
  bool lockedPrimaryReal = false;
  bool secondaryRuntimeAllowed = true;
  const char *reason = "secondary_simulation_runtime";
};

class RuntimePathLock {
public:
  RuntimePathDecision decide(const NeuralChipStatus &status,
                             RuntimeDeploymentMode deploymentMode);
  RuntimePathDecision current() const;
  bool primaryRealLocked() const;

private:
  RuntimePathDecision decision_{};
};

} // namespace dnxr
