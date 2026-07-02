#pragma once
#include "full_dive/spike_bridge/VectorIntentNormalizer.h"
namespace dnxr {
class OlfactoryVectorToSpikeAdapter {
public:
  ModalityVectorPacket adapt(std::uint64_t frameId, double timestampMs,
                             const NeuralOlfactoryVector &vector) const;
};
} // namespace dnxr
