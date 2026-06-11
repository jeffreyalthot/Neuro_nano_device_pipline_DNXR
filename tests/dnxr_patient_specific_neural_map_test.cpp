#include "neural_chip/PatientSpecificNeuralMap.h"
#include <cassert>

int main() {
    const auto map = dnxr::PatientSpecificNeuralMap::simulationDefaults();
    assert(map.motor.enabled);
    assert(map.touch.enabled);
    assert(map.proprioception.enabled);
    assert(map.motor.maturity == dnxr::NeuralChannelMaturity::SupportedSimulation);
    assert(!map.visual.enabled);
    assert(map.visual.maturity == dnxr::NeuralChannelMaturity::ResearchOnly);
    assert(!map.auditory.enabled);
    assert(map.auditory.maturity == dnxr::NeuralChannelMaturity::ResearchOnly);
    assert(!map.vestibular.enabled);
    assert(map.vestibular.maturity == dnxr::NeuralChannelMaturity::ResearchOnly);
    assert(!map.olfactory.enabled);
    assert(!map.gustatory.enabled);
    assert(map.olfactory.maturity == dnxr::NeuralChannelMaturity::Experimental);
    assert(map.gustatory.maxAllowed01 == 0.80f);
    return 0;
}
