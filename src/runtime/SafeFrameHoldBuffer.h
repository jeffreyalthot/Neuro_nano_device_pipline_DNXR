#pragma once
#include "../full_dive/FullDiveNeuralFrame.h"
namespace dnxr { class SafeFrameHoldBuffer { public: void rememberIfSafe(const FullDiveNeuralFrame& frame); FullDiveNeuralFrame holdWithoutRiskyModalities() const; bool hasSafeFrame() const; private: FullDiveNeuralFrame last_; bool has_=false; }; }
