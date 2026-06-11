#pragma once
#include "neural_chip/NeuralChipFrame.h"
namespace dnxr { class PhospheneFrameStabilizer { public: NeuralVisualVector stabilize(const NeuralVisualVector& current) const; private: static float clamp01(float value); }; }
