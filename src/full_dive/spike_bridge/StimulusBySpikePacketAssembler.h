#pragma once
#include "full_dive/spike_bridge/SpikeBridgeTypes.h"
namespace dnxr {
class StimulusBySpikePacketAssembler {
public:
  FullDiveChipFrame
  assemble(const StimulusBySpikeFrame &, const NeuralVisualVector &,
           const NeuralAuditoryVector &, const NeuralTouchVector &,
           const NeuralProprioceptionVector &, const NeuralVestibularVector &,
           const NeuralMotorIntentVector &, const NeuralOlfactoryVector &,
           const NeuralGustatoryVector &, const NeuralRealityAnchorVector &,
           const NeuralSafetyVector &) const;
};
} // namespace dnxr
