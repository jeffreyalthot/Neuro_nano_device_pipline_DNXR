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
  PPPGrammarSerializedFrame serialize(const PPPGrammarFrame &frame) const;
  PPPGrammarFrame parse(const PPPGrammarSerializedFrame &serialized) const;
  bool verify(const PPPGrammarFrame &frame) const;
};

} // namespace dnxr
