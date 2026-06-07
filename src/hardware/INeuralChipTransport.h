#pragma once

#include "RealNeuralChipPacket.h"

#include <cstdint>
#include <string>

namespace dnxr {

struct NeuralTransportAck {
    bool accepted = false;
    bool applied = false;
    std::string rejectedReason;
    float measuredLatencyMs = 0.0f;
    std::uint64_t feedbackFrameId = 0;
    bool hardwareCutoffState = false;
};

class INeuralChipTransport {
public:
    virtual ~INeuralChipTransport() = default;
    virtual NeuralTransportAck send(const RealNeuralChipPacket& packet) = 0;
    virtual bool cutoff(const std::string& reason) = 0;
};

} // namespace dnxr
