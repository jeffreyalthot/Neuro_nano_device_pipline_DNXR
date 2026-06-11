#pragma once

#include "neural_chip/NeuralChipReplayRecorder.h"

namespace dnxr {

struct ReplayConsistencyReport {
    bool valid = false;
    const char* reason = "";
    std::size_t recordsChecked = 0;
};

class ReplayConsistencyVerifier {
public:
    ReplayConsistencyReport verify(const NeuralChipReplayRecorder& recorder) const;
};

} // namespace dnxr
