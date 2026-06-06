#pragma once
#include "FullDiveNeuralFrame.h"
#include <string>
#include <vector>
namespace dnxr { struct ExpectedPerception { std::string description; float expectedResponse=0.0f; float tolerance=0.15f; }; class ExpectedPerceptionModel { public: std::vector<ExpectedPerception> predict(const FullDiveNeuralFrame& frame) const; }; }
