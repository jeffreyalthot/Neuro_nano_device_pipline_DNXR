#pragma once
#include "FullDiveNeuralFrame.h"
namespace dnxr { class FrameDropCompensator { public: FullDiveNeuralFrame compensate(const FullDiveNeuralFrame& lastSafeFrame, bool proprioceptionDrift) const; }; }
