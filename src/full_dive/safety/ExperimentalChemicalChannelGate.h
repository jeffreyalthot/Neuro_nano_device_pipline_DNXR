#pragma once

#include "neural_chip/NeuralChipFrame.h"

namespace dnxr {

class ExperimentalChemicalChannelGate {
public:
    void setEnabled(bool enabled);
    FullDiveChipFrame apply(const FullDiveChipFrame& frame) const;
    bool enabled() const;

private:
    static float clampMax(float value, float maximum);
    bool enabled_ = false;
};

} // namespace dnxr
