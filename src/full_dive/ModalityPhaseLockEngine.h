#pragma once
#include "FullDiveNeuralFrame.h"
#include <map>
#include <array>
namespace dnxr { struct ModalityPhaseState { std::uint64_t frameTimestamp=0; float expectedArrivalMs=0, actualArrivalMs=0, jitterMs=0, phaseError=0, latencySkew=0, confidence=1; }; struct PhaseLockReport { std::map<FullDiveModality,ModalityPhaseState> modalities; bool locked=true; float worstSkewMs=0; }; class ModalityPhaseLockEngine { public: PhaseLockReport evaluate(const FullDiveNeuralFrame& frame, const std::map<FullDiveModality,float>& arrivalsMs) const; PhaseLockReport evaluate(const FullDiveNeuralFrame& frame, const std::array<float,8>& arrivalsMs) const; }; }
