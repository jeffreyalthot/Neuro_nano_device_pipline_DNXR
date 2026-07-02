#include "full_dive/gustatory_chip/GustatoryVectorToSpikeAdapter.h"
namespace dnxr {
ModalityVectorPacket GustatoryVectorToSpikeAdapter::adapt(
    std::uint64_t frameId, double timestampMs,
    const NeuralGustatoryVector &vector) const {
  return VectorIntentNormalizer{}.fromGustatory(frameId, timestampMs, vector);
}
} // namespace dnxr
