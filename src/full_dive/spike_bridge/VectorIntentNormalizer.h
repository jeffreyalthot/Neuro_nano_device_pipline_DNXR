#pragma once
#include "full_dive/spike_bridge/SpikeBridgeTypes.h"
namespace dnxr {
class VectorIntentNormalizer {
public:
  VectorBundleFrame
  normalize(std::uint64_t frameId, double timestampMs,
            const NeuralVisualVector &, const NeuralAuditoryVector &,
            const NeuralTouchVector &, const NeuralProprioceptionVector &,
            const NeuralVestibularVector &, const NeuralMotorIntentVector &,
            const NeuralOlfactoryVector &, const NeuralGustatoryVector &,
            const NeuralRealityAnchorVector &) const;
  ModalityVectorPacket fromVisual(std::uint64_t, double,
                                  const NeuralVisualVector &) const;
  ModalityVectorPacket fromAuditory(std::uint64_t, double,
                                    const NeuralAuditoryVector &) const;
  ModalityVectorPacket fromTouch(std::uint64_t, double,
                                 const NeuralTouchVector &) const;
  ModalityVectorPacket
  fromProprioception(std::uint64_t, double,
                     const NeuralProprioceptionVector &) const;
  ModalityVectorPacket fromVestibular(std::uint64_t, double,
                                      const NeuralVestibularVector &) const;
  ModalityVectorPacket fromMotor(std::uint64_t, double,
                                 const NeuralMotorIntentVector &) const;
  ModalityVectorPacket fromOlfactory(std::uint64_t, double,
                                     const NeuralOlfactoryVector &) const;
  ModalityVectorPacket fromGustatory(std::uint64_t, double,
                                     const NeuralGustatoryVector &) const;
  ModalityVectorPacket
  fromRealityAnchor(std::uint64_t, double,
                    const NeuralRealityAnchorVector &) const;
};
} // namespace dnxr
