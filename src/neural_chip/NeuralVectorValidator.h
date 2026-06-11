#pragma once

#include "neural_chip/NeuralChipFrame.h"

namespace dnxr {

struct NeuralVectorValidationResult {
    bool accepted = false;
    const char* reason = "not validated";
};

class NeuralVectorValidator {
public:
    static NeuralVectorValidationResult validate(const FullDiveChipFrame& frame);

    static bool isUnitInterval(float value);
};

} // namespace dnxr
