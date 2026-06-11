#pragma once

#include "hardware/ChipCapabilityNegotiation.h"
#include "neural_chip/NeuralChipFrame.h"

namespace dnxr {

class ChipCapabilityContract {
public:
    explicit ChipCapabilityContract(ChipCapabilities capabilities = {});
    FullDiveChipFrame apply(const FullDiveChipFrame& frame) const;
    bool requiresSafetyHold() const;
    void setCapabilities(ChipCapabilities capabilities);

private:
    ChipCapabilities capabilities_{};
};

} // namespace dnxr
