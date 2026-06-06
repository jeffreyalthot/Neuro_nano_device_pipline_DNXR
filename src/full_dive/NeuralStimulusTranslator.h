#pragma once
#include "FullDiveNeuralFrame.h"
#include "NeuralTargetMap.h"
#include "SensoryEncodingModelRegistry.h"
#include "../hardware/NeuralChipHAL.h"
#include "../safety/NeuralStimulusSafetyEnvelope.h"
namespace dnxr {
struct NeuralStimulusTranslationResult { SafeNeuralStimulusCommand command; bool valid = false; std::string reason = "not_translated"; };
class NeuralStimulusTranslator {
public:
    NeuralStimulusTranslator();
    NeuralStimulusTranslationResult translate(const FullDiveNeuralFrame& frame) const;
private:
    NeuralTargetMap targetMap_;
};
} // namespace dnxr
