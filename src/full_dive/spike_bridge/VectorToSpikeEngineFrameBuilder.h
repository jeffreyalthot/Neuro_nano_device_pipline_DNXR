#pragma once
#include "full_dive/spike_bridge/SpikeBridgeTypes.h"
namespace dnxr {
class VectorToSpikeEngineFrameBuilder {
public:
  SpikeEngineIntentFrame build(const VectorBundleFrame &input) const;
};
} // namespace dnxr
