#pragma once
#include "full_dive/spike_bridge/SpikeBridgeTypes.h"
#include "neural_chip/PatientSpecificNeuralMap.h"
namespace dnxr {
class PatientTargetBindingLayer {
public:
  VectorBundleFrame bind(const VectorBundleFrame &input,
                         const PatientSpecificNeuralMap &map) const;
};
} // namespace dnxr
