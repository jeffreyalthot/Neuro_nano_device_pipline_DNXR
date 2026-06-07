#pragma once

#include "NeuralChipAck.h"
#include "NeuralChipHAL.h"

#include <string>

namespace dnxr {

enum class NeuralChipSafetyAction {
    Allow,
    ReduceIntensity,
    HoldLastSafeFrame,
    SuppressModality,
    ForceRealityAnchor,
    RequestDecompression,
    EmergencyStop
};

struct NeuralChipSafetyDecision {
    bool allowed = true;
    NeuralChipSafetyAction action = NeuralChipSafetyAction::Allow;
    std::string reason = "neural_chip_safe";
    float severity = 0.0f;
    bool requiresEmergencyStop = false;
};

class NeuralChipSafetySupervisor {
public:
    NeuralChipSafetyDecision evaluate(const NeuralChipAck& ack,
                                      const ChipCapabilities& currentCapabilities,
                                      const DeviceIdentity& identity,
                                      std::uint64_t expectedPacketId,
                                      const ChipCapabilities* sessionCapabilities = nullptr) const;
    NeuralChipSafetyDecision evaluate(const NeuralChipAck& ack, bool cutoffAvailable, std::uint64_t expectedPacketId) const;
};

} // namespace dnxr
