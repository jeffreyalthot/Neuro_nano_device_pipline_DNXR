#pragma once

#include "neural_chip/NeuralChipReplayRecorder.h"

#include <string>

namespace dnxr {

struct ReplayConsistencyReport {
    bool valid = false;
    std::string reason;
    std::size_t recordsChecked = 0;
};

class ReplayConsistencyVerifier {
public:
    ReplayConsistencyReport verify(const NeuralChipReplayRecorder& recorder) const;
};

} // namespace dnxr
