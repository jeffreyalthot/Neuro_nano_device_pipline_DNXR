#include "full_dive/touch_chip/TouchVectorToSpikeAdapter.h"
namespace dnxr {
ModalityVectorPacket
TouchVectorToSpikeAdapter::adapt(std::uint64_t frameId, double timestampMs,
                                 const NeuralTouchVector &vector) const {
  return VectorIntentNormalizer{}.fromTouch(frameId, timestampMs, vector);
}
} // namespace dnxr
