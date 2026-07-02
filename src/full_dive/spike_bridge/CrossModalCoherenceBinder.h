#pragma once
#include "full_dive/spike_bridge/SpikeBridgeTypes.h"
namespace dnxr {
class CrossModalCoherenceBinder {
public:
  VectorBundleFrame bind(const VectorBundleFrame &input) const;
};
} // namespace dnxr
