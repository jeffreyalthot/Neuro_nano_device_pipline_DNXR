#pragma once
#include "full_dive/spike_bridge/VectorIntentNormalizer.h"
namespace dnxr {
class GustatoryVectorToSpikeAdapter {
public:
  ModalityVectorPacket adapt(std::uint64_t frameId, double timestampMs,
                             const NeuralGustatoryVector &vector) const;
};
} // namespace dnxr
