#include "full_dive/olfactory_chip/OlfactoryVectorToSpikeAdapter.h"
namespace dnxr {
ModalityVectorPacket OlfactoryVectorToSpikeAdapter::adapt(
    std::uint64_t frameId, double timestampMs,
    const NeuralOlfactoryVector &vector) const {
  return VectorIntentNormalizer{}.fromOlfactory(frameId, timestampMs, vector);
}
} // namespace dnxr
