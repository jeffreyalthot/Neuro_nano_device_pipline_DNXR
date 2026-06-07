#include "../../src/full_dive/NeuralStimulusTranslator.h"
#include <cassert>
#include <type_traits>
int main(){ dnxr::NeuralStimulusTranslator t; dnxr::FullDiveNeuralFrame f; auto r=t.translate(f); static_assert(std::is_same<decltype(r.command), dnxr::SafeNeuralStimulusCommandFixed>::value, "translator must return fixed command"); assert(r.command.modalityIntensities.size()==static_cast<std::size_t>(dnxr::SensoryModality::Count)); return 0; }
