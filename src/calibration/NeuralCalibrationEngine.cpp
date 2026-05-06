#include "NeuralCalibrationEngine.h"
namespace dnxr { std::vector<float> NeuralCalibrationEngine::calibrateForImmersion(const std::vector<float>& features,float userSensitivity) const { return adaptation_.applyUserProfile(features,userSensitivity); } }
