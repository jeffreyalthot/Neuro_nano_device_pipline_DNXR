#include "full_dive/touch_chip/ProprioceptionVectorToSpikeAdapter.h"
namespace dnxr {
ModalityVectorPacket ProprioceptionVectorToSpikeAdapter::adapt(
    std::uint64_t frameId, double timestampMs,
    const NeuralProprioceptionVector &vector) const {
  return VectorIntentNormalizer{}.fromProprioception(frameId, timestampMs,
                                                     vector);
}
} // namespace dnxr
