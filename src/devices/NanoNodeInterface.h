#pragma once
#include <vector>
namespace dnxr { class NanoNodeInterface { public: std::vector<float> readNeuralNodeSignals() const; void writeNeuralStimulus(const std::vector<float>& vec) const; }; }
