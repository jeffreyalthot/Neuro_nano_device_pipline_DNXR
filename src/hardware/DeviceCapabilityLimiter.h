#pragma once

#include "ChipCapabilityNegotiation.h"
#include "../full_dive/FullDiveNeuralFrame.h"

#include <string>

namespace dnxr {

struct DeviceCapabilityLimitResult {
    FullDiveNeuralFrame frame;
    bool allowed = true;
    std::string reason = "capabilities_ok";
};

class DeviceCapabilityLimiter {
public:
    DeviceCapabilityLimitResult apply(const FullDiveNeuralFrame& frame, const ChipCapabilities& capabilities, float runtimeRateHz) const;
};

} // namespace dnxr
