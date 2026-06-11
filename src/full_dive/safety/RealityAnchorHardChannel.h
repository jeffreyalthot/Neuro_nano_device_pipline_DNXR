#pragma once

#include "neural_chip/NeuralChipFrame.h"

namespace dnxr {

class RealityAnchorHardChannel {
public:
    bool validates(const NeuralRealityAnchorVector& anchor) const;
    FullDiveChipFrame enforce(const FullDiveChipFrame& frame) const;
};

} // namespace dnxr
