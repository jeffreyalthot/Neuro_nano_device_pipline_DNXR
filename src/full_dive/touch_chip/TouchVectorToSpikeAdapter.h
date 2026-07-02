#pragma once
#include "full_dive/spike_bridge/VectorIntentNormalizer.h"
namespace dnxr {
class TouchVectorToSpikeAdapter {
public:
  ModalityVectorPacket adapt(std::uint64_t frameId, double timestampMs,
                             const NeuralTouchVector &vector) const;
};
} // namespace dnxr
