#include "neural_chip/PatientSpecificNeuralMap.h"

namespace dnxr {
namespace {
PatientModalityProfile profile(NeuralChannelMaturity maturity, bool enabled) {
    PatientModalityProfile out;
    out.enabled = enabled;
    out.maturity = maturity;
    out.maxAllowed01 = 0.80f;
    out.confidence01 = enabled ? 0.5f : 0.0f;
    return out;
}
} // namespace

PatientSpecificNeuralMap PatientSpecificNeuralMap::simulationDefaults() {
    PatientSpecificNeuralMap map;
    map.motor = profile(NeuralChannelMaturity::SupportedSimulation, true);
    map.touch = profile(NeuralChannelMaturity::SupportedSimulation, true);
    map.proprioception = profile(NeuralChannelMaturity::SupportedSimulation, true);
    map.visual = profile(NeuralChannelMaturity::ResearchOnly, false);
    map.auditory = profile(NeuralChannelMaturity::ResearchOnly, false);
    map.vestibular = profile(NeuralChannelMaturity::ResearchOnly, false);
    map.olfactory = profile(NeuralChannelMaturity::Experimental, false);
    map.gustatory = profile(NeuralChannelMaturity::Experimental, false);
    map.visualChip.retinotopicConfidence01 = 0.0f;
    map.chemicalChip.allowedComplexity01 = 0.0f;
    return map;
}

} // namespace dnxr
