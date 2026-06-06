#include "MotorIntentClosedLoop.h"
#include <algorithm>
#include <cmath>
namespace dnxr { MotorIntentLoopState MotorIntentClosedLoop::update(const std::string& intentClass, float intentStrength, const FullDiveNeuralFrame& expected, float measuredResponse) const { MotorIntentLoopState s; s.predictedAction=intentClass; s.executedAction=intentStrength>0.35f?intentClass:"no_action_low_confidence"; s.expectedSensoryReturn=expected; s.actualNeuralResponse=measuredResponse; s.predictionError=std::fabs(expected.maxIntensity()-measuredResponse); s.confidence=std::clamp(intentStrength*(1.0f-s.predictionError),0.0f,1.0f); s.rollbackRequired=s.predictionError>0.45f || s.confidence<0.25f; return s; } }
