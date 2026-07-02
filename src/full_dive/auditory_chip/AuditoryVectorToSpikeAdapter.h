#pragma once
#include "full_dive/spike_bridge/VectorIntentNormalizer.h"
namespace dnxr {
class AuditoryVectorToSpikeAdapter {
public:
  ModalityVectorPacket adapt(std::uint64_t frameId, double timestampMs,
                             const NeuralAuditoryVector &vector) const;
};
} // namespace dnxr
