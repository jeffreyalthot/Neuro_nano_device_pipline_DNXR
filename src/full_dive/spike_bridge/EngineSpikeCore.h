#pragma once
#include "full_dive/spike_bridge/SpikeBridgeTypes.h"
namespace dnxr {
class EngineSpikeCore {
public:
  EngineSpikeFrame process(const SpikeEngineIntentFrame &intent) const;
};
} // namespace dnxr
