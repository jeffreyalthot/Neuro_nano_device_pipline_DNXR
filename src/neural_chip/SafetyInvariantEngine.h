#pragma once

#include "neural_chip/ChipCapabilityContract.h"
#include "neural_chip/NeuralChipAckHandshake.h"
#include "neural_chip/NeuralChipFrame.h"
#include "neural_chip/NeuralChipSessionState.h"

namespace dnxr {

struct SafetyInvariantResult {
    bool accepted = true;
    const char* reason = "accepted";
    bool triggerEmergencyLatch = false;
    bool requireHold = false;
};

class SafetyInvariantEngine {
public:
    SafetyInvariantResult validate(const FullDiveChipFrame& frame,
                                   const NeuralChipSessionState& session,
                                   const NeuralChipAckState& ack,
                                   const ChipCapabilities& capabilities) const;
};

} // namespace dnxr
