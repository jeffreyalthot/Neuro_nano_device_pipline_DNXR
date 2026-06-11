#pragma once

#include "neural_chip/NeuralChipFrame.h"

namespace dnxr {

class PerModalityIsolationBroker {
public:
    FullDiveChipFrame assemble(NeuralVisualVector visual,
                               NeuralAuditoryVector auditory,
                               NeuralTouchVector touch,
                               NeuralProprioceptionVector proprioception,
                               NeuralVestibularVector vestibular,
                               NeuralMotorIntentVector motor,
                               NeuralOlfactoryVector olfactory,
                               NeuralGustatoryVector gustatory,
                               std::uint64_t frameId = 0,
                               double timestampMs = 0.0) const;
};

} // namespace dnxr
