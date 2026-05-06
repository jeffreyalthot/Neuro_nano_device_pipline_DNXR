#pragma once
#include "AdaptationLayer.h"
#include <vector>
namespace dnxr { class NeuralCalibrationEngine { public: std::vector<float> calibrateForImmersion(const std::vector<float>& features,float userSensitivity) const; private: AdaptationLayer adaptation_; }; }
