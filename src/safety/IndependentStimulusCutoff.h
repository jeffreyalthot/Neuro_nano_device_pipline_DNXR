#pragma once

#include "../hardware/NeuralChipHAL.h"

#include <string>

namespace dnxr {

struct StimulusCutoffDecision {
    bool outputZeroed = true;
    bool emergencyLatched = false;
    std::string reason = "clinical_lockdown";
    SafeNeuralStimulusCommand command;
};

class IndependentStimulusCutoff {
public:
    StimulusCutoffDecision zeroAllOutputs(const SafeNeuralStimulusCommand& requested,
                                          const std::string& reason) const;
};

} // namespace dnxr
