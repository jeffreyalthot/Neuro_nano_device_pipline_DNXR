#pragma once

#include "NeuralChipHAL.h"
#include "ClosedLoopFeedbackFrame.h"
#include "../full_dive/PerModalityFeedbackAnalyzer.h"

namespace dnxr {

struct ClosedLoopFeedback {
    bool available = false;
    float perceptionConfidence = 0.0f;
    float adjustment = 0.0f;
    bool safetyVerified = false;
    ClosedLoopFeedbackFrame perModality;
    PerModalityFeedbackAnalysis modalityAnalysis;
};

class ClosedLoopNeuralFeedbackReader {
public:
    ClosedLoopFeedback read(const NeuralChipFrame& response, const SafeNeuralStimulusCommand& stimulus) const;
};

} // namespace dnxr
