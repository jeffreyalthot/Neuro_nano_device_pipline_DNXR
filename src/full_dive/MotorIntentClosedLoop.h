#pragma once
#include "FullDiveNeuralFrame.h"
#include <string>
namespace dnxr {
struct MotorIntentLoopState { std::string predictedAction; std::string executedAction; FullDiveNeuralFrame expectedSensoryReturn; float actualNeuralResponse=0.0f; float predictionError=0.0f; float confidence=0.0f; bool rollbackRequired=false; };
class MotorIntentClosedLoop { public: MotorIntentLoopState update(const std::string& intentClass, float intentStrength, const FullDiveNeuralFrame& expected, float measuredResponse) const; };
} // namespace dnxr
