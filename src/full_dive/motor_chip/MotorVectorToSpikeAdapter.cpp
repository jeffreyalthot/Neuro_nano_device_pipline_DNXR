#include "full_dive/motor_chip/MotorVectorToSpikeAdapter.h"
namespace dnxr {
ModalityVectorPacket
MotorVectorToSpikeAdapter::adapt(std::uint64_t frameId, double timestampMs,
                                 const NeuralMotorIntentVector &vector) const {
  return VectorIntentNormalizer{}.fromMotor(frameId, timestampMs, vector);
}
} // namespace dnxr
