#pragma once
#include "NeuralChipHAL.h"
namespace dnxr { struct ClosedLoopFeedback { bool available=false; float perceptionConfidence=0.0f; float adjustment=0.0f; bool safetyVerified=false; }; class ClosedLoopNeuralFeedbackReader { public: ClosedLoopFeedback read(const NeuralChipFrame& response, const SafeNeuralStimulusCommand& stimulus) const; }; }
