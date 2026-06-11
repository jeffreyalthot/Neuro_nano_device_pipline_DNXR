#pragma once

#include "neural_chip/NeuralChipFrame.h"

namespace dnxr {

class PerModalitySafetyEnvelope {
public:
    FullDiveChipFrame apply(const FullDiveChipFrame& frame) const;
    bool validates(const FullDiveChipFrame& frame) const;

private:
    static float clamp01(float value);
    static float capActive(float value, float ceiling);
};

} // namespace dnxr
