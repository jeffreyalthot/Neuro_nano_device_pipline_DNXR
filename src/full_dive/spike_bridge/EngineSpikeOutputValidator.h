#pragma once
#include "full_dive/spike_bridge/SpikeBridgeTypes.h"
namespace dnxr {
class EngineSpikeOutputValidator {
public:
  EngineSpikeFrame validate(const EngineSpikeFrame &input) const;
};
} // namespace dnxr
