#pragma once
#include <vector>
namespace dnxr { class NeuralSignalEmulator { public: std::vector<double> generate(std::size_t channels, double amplitude) const; }; }
