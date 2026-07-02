#pragma once
#include "full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.h"
namespace dnxr {
struct EnvironmentSyncNode {
  std::uint64_t frameId = 0;
  double timestampMs = 0;
  std::uint64_t sceneRevision = 0, avatarPoseRevision = 0,
                audioEventRevision = 0, contactRevision = 0,
                chemicalEventRevision = 0;
  float visualCoherence01 = 1, auditoryCoherence01 = 1, touchCoherence01 = 1,
        motorCoherence01 = 1, vestibularCoherence01 = 1,
        olfactoryCoherence01 = 1, gustatoryCoherence01 = 1,
        globalCoherence01 = 1;
};
class FullDiveEnvironmentSynchronizationGraph {
public:
  EnvironmentSyncNode buildNode(const NeuralChipOnlyWorldFrame &world) const;
  float compare(const EnvironmentSyncNode &previous,
                const EnvironmentSyncNode &current) const;
  bool requiresHold(const EnvironmentSyncNode &node) const;
  const char *holdReason(const EnvironmentSyncNode &node) const;
};
} // namespace dnxr
