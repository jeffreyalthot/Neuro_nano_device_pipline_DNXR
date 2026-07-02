#include "full_dive/touch_chip/VestibularVectorToSpikeAdapter.h"
namespace dnxr {
ModalityVectorPacket VestibularVectorToSpikeAdapter::adapt(
    std::uint64_t frameId, double timestampMs,
    const NeuralVestibularVector &vector) const {
  return VectorIntentNormalizer{}.fromVestibular(frameId, timestampMs, vector);
}
} // namespace dnxr
