#pragma once

#include "neural_chip/NeuralChipFrame.h"

#include <vector>

namespace dnxr {

struct FrameFieldDelta {
    const char* modality = "";
    const char* fieldPath = "";
    float before01 = 0.0f;
    float after01 = 0.0f;
    float delta01 = 0.0f;
};

class FullDiveChipFrameDiff {
public:
    static std::vector<FrameFieldDelta> compare(const FullDiveChipFrame& before, const FullDiveChipFrame& after, float epsilon01 = 0.0001f);
};

} // namespace dnxr
