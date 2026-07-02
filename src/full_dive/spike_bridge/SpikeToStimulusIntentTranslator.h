#pragma once

#include "full_dive/spike_bridge/SpikeBridgeTypes.h"

namespace dnxr {

class SpikeToStimulusIntentTranslator {
public:
  StimulusBySpikeFrame translate(const EngineSpikeFrame &frame) const;
};

} // namespace dnxr
