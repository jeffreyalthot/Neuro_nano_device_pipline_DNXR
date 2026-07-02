#pragma once

#include "full_dive/spike_bridge/SpikeBridgeTypes.h"
#include "hardware/NeuralChipHAL.h"

namespace dnxr {

class StimulusFeedbackSynchronizer {
public:
  StimulusBySpikeFrame synchronizeFeedback(const StimulusBySpikeFrame &input,
                                           const ChipReadFrame &read,
                                           const NeuralChipAck &ack);
  float feedbackQuality01() const;
  bool requiresHold() const;

private:
  float quality_ = 1.0f;
  bool hold_ = false;
};

} // namespace dnxr
