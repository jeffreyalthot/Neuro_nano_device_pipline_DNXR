#pragma once

#include "full_dive/spike_bridge/SpikeBridgeTypes.h"

namespace dnxr {

class MultiLayerSpikeThinkingEngine {
public:
  SpikeEngineIntentFrame think(const SpikeEngineIntentFrame &intent,
                               const PPPGrammarFrame &grammar,
                               MultiThinkingFrame *thinkingFrame) const;
};

} // namespace dnxr
