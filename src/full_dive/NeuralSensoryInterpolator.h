#pragma once
#include "FullDiveNeuralFrame.h"
namespace dnxr { class NeuralSensoryInterpolator { public: FullDiveNeuralFrame interpolate(const FullDiveNeuralFrame& previous, const FullDiveNeuralFrame& next, float alpha, bool emergency=false) const; }; }
