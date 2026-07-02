#include "full_dive/auditory_chip/AuditoryVectorToSpikeAdapter.h"
namespace dnxr {
ModalityVectorPacket
AuditoryVectorToSpikeAdapter::adapt(std::uint64_t frameId, double timestampMs,
                                    const NeuralAuditoryVector &vector) const {
  return VectorIntentNormalizer{}.fromAuditory(frameId, timestampMs, vector);
}
} // namespace dnxr
