#pragma once
#include "ExpectedPerceptionModel.h"
#include "../hardware/ClosedLoopNeuralFeedbackReader.h"
#include <string>
#include <vector>
namespace dnxr { struct PerceptionDivergenceReport { float divergenceScore=0; std::vector<FullDiveModality> affectedModalities; std::string likelyCause="none",recommendedCorrection="none"; bool mustStop=false; }; class PerceptionDivergenceDetector { public: PerceptionDivergenceReport detect(const std::vector<ExpectedPerception>& expected, const ClosedLoopFeedback& actual, float physiologicalStress, float latencyMs) const; }; }
