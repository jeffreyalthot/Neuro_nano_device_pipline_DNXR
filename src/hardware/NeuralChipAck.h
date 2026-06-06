#pragma once

#include <cstdint>
#include <string>

namespace dnxr {

struct NeuralChipAck {
    std::uint64_t packetId = 0;
    bool packetAccepted = false;
    bool packetRejected = false;
    bool applied = false;
    bool notApplied = true;
    bool hardwareCutoffActive = false;
    float latencyMeasuredMs = 0.0f;
    float deviceTemperature = 37.0f;
    float signalQuality = 0.0f;
    bool feedbackAvailable = false;
    bool signedAck = false;
    std::string rejectReason;
};

} // namespace dnxr
