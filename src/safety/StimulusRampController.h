#pragma once
#include "../hardware/NeuralChipHAL.h"
namespace dnxr { class StimulusRampController { public: SafeNeuralStimulusCommand apply(const SafeNeuralStimulusCommand& previous, const SafeNeuralStimulusCommand& next, float maxDeltaPerFrame, bool emergency=false) const; }; }
