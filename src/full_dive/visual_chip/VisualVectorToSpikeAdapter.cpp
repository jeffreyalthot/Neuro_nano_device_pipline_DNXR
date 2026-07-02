#include "full_dive/visual_chip/VisualVectorToSpikeAdapter.h"
namespace dnxr {
ModalityVectorPacket
VisualVectorToSpikeAdapter::adapt(std::uint64_t frameId, double timestampMs,
                                  const NeuralVisualVector &vector) const {
  return VectorIntentNormalizer{}.fromVisual(frameId, timestampMs, vector);
}
} // namespace dnxr
