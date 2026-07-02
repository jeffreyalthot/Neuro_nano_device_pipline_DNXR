#pragma once
#include "full_dive/spike_bridge/SpikeBridgeTypes.h"
#include "neural_chip/NeuralChipSessionState.h"
namespace dnxr {
class SpikeEnvelopePolicyApplier {
public:
  EngineSpikeFrame apply(const EngineSpikeFrame &input,
                         const NeuralChipSessionState &session) const;
};
} // namespace dnxr
