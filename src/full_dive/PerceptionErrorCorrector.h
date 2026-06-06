#pragma once
#include "FullDiveNeuralFrame.h"
namespace dnxr { struct PerceptionCorrectionPolicy { float patientLearningRate=0.05f; float maxCorrectionPerSecond=0.10f; float antiRunawayClamp=0.20f; }; class PerceptionErrorCorrector { public: FullDiveNeuralFrame correct(const FullDiveNeuralFrame& frame, float predictionError, double dtSeconds, const PerceptionCorrectionPolicy& policy={}) const; }; }
