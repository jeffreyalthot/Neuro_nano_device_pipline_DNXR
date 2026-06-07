#pragma once

#include "FullDiveNeuralFrame.h"
#include "../hardware/ClosedLoopFeedbackFrame.h"

namespace dnxr {

class ExpectedNeuralResponseModel {
public:
    ClosedLoopFeedbackFrame expectedFromStimulus(const FullDiveNeuralFrame& frame) const;
};

} // namespace dnxr
