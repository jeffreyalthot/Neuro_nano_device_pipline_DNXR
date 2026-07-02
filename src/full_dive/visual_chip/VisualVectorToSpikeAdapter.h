#pragma once
#include "full_dive/spike_bridge/VectorIntentNormalizer.h"
namespace dnxr {
class VisualVectorToSpikeAdapter {
public:
  ModalityVectorPacket adapt(std::uint64_t frameId, double timestampMs,
                             const NeuralVisualVector &vector) const;
};
} // namespace dnxr
