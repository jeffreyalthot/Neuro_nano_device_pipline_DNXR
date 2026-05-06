#pragma once
#include <vector>
namespace dnxr { class SignalStreamProcessor { public: std::vector<float> denoise(const std::vector<float>& raw) const; }; }
