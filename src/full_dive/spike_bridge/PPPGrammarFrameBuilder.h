#pragma once

#include "full_dive/spike_bridge/SpikeBridgeTypes.h"
#include "hardware/NeuralChipAck.h"
#include "neural_chip/NeuralChipSessionState.h"

namespace dnxr {

class PPPGrammarFrameBuilder {
public:
  PPPGrammarFrame build(const VectorBundleFrame &vectors,
                        const NeuralChipSessionState &session,
                        const NeuralChipAck &ack) const;
};

} // namespace dnxr
