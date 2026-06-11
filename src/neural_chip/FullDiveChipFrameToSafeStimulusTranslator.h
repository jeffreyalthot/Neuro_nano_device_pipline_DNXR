#pragma once

#include "hardware/NeuralChipHAL.h"
#include "neural_chip/NeuralChipFrame.h"

namespace dnxr {

enum class FullDiveStimulusCoveragePolicy { RequiredModalities, FullMultisensoryFrame };

class FullDiveChipFrameToSafeStimulusTranslator {
public:
    SafeNeuralStimulusCommandFixed translate(const FullDiveChipFrame& frame,
                                             FullDiveStimulusCoveragePolicy policy = FullDiveStimulusCoveragePolicy::RequiredModalities) const;

private:
    static float clamp01(float value);
    static void append(SafeNeuralStimulusCommandFixed& command, float value);
};

} // namespace dnxr
