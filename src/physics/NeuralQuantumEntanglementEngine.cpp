#include "NeuralQuantumEntanglementEngine.h"

#include <algorithm>
#include <cmath>

namespace neuroxr {

NeuralQuantumEntanglementEngine::
NeuralQuantumEntanglementEngine() {

    initialize();
}

void NeuralQuantumEntanglementEngine::
initialize() {

    nextStateId_ = 1;

    frame_.globalCoherence =
        1.0f;

    frame_.neuralEntanglement =
        0.0f;

    frame_.quantumStability =
        1.0f;

    frame_.temporalIntegrity =
        1.0f;

    frame_.consciousnessCoupling =
        0.0f;

    frame_.predictiveResonance =
        0.0f;

    frame_.uncertaintyFactor =
        0.0f;

    frame_.decoherenceRisk =
        0.0f;
}

uint64_t
NeuralQuantumEntanglementEngine::
createQuantumState(
    QuantumEntanglementType type) {

    QuantumNeuralState state;

    state.stateId =
        nextStateId_++;

    state.type =
        type;

    state.coherence =
        1.0f;

    state.uncertainty =
        0.0f;

    state.entanglementStrength =
        0.0f;

    state.waveStability =
        1.0f;

    state.temporalAlignment =
        1.0f;

    state.quantumSignature.resize(
        128,
        0.5f);

    states_[state.stateId] =
        state;

    return state.stateId;
}

void
NeuralQuantumEntanglementEngine::
entangleStates(
    uint64_t stateA,
    uint64_t stateB,
    float strength) {

    auto itA =
        states_.find(stateA);

    auto itB =
        states_.find(stateB);

    if(itA == states_.end() ||
       itB == states_.end()) {

        return;
    }

    itA->second.entanglementStrength +=
        strength;

    itB->second.entanglementStrength +=
        strength;

    if(itA->second.entanglementStrength >
        1.0f) {

        itA->second.entanglementStrength =
            1.0f;
    }

    if(itB->second.entanglementStrength >
        1.0f) {

        itB->second.entanglementStrength =
            1.0f;
    }
}

void
NeuralQuantumEntanglementEngine::
computeGlobalCoherence() {

    float coherence = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        states_) {

        coherence +=
            pair.second.coherence;

        ++count;
    }

    if(count > 0) {

        coherence /=
            static_cast<float>(
                count);
    }

    frame_.globalCoherence =
        coherence;
}

void
NeuralQuantumEntanglementEngine::
computeNeuralEntanglement() {

    float entanglement = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        states_) {

        entanglement +=
            pair.second.entanglementStrength;

        ++count;
    }

    if(count > 0) {

        entanglement /=
            static_cast<float>(
                count);
    }

    frame_.neuralEntanglement =
        entanglement;
}

void
NeuralQuantumEntanglementEngine::
computeQuantumStability() {

    float stability = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        states_) {

        stability +=
            pair.second.waveStability;

        ++count;
    }

    if(count > 0) {

        stability /=
            static_cast<float>(
                count);
    }

    frame_.quantumStability =
        stability;
}

void
NeuralQuantumEntanglementEngine::
computeTemporalIntegrity() {

    float integrity = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        states_) {

        integrity +=
            pair.second.temporalAlignment;

        ++count;
    }

    if(count > 0) {

        integrity /=
            static_cast<float>(
                count);
    }

    frame_.temporalIntegrity =
        integrity;
}

void
NeuralQuantumEntanglementEngine::
computeConsciousnessCoupling() {

    frame_.consciousnessCoupling =

        frame_.globalCoherence *

        frame_.neuralEntanglement;
}

void
NeuralQuantumEntanglementEngine::
computePredictiveResonance() {

    frame_.predictiveResonance =

        frame_.consciousnessCoupling *

        frame_.temporalIntegrity;
}

void
NeuralQuantumEntanglementEngine::
computeUncertaintyFactor() {

    float uncertainty = 0.0f;

    uint64_t count = 0;

    for(const auto& pair :
        states_) {

        uncertainty +=
            pair.second.uncertainty;

        ++count;
    }

    if(count > 0) {

        uncertainty /=
            static_cast<float>(
                count);
    }

    frame_.uncertaintyFactor =
        uncertainty;
}

void
NeuralQuantumEntanglementEngine::
computeDecoherenceRisk() {

    frame_.decoherenceRisk =

        frame_.uncertaintyFactor *

        (1.0f -
        frame_.quantumStability);
}

void
NeuralQuantumEntanglementEngine::
stabilizeQuantumField() {

    for(auto& pair :
        states_) {

        pair.second.waveStability =

            frame_.quantumStability;

        pair.second.coherence =

            frame_.globalCoherence;
    }
}

void
NeuralQuantumEntanglementEngine::
collapseUnstableStates() {

    for(auto& pair :
        states_) {

        if(pair.second.waveStability <
            0.15f) {

            pair.second.coherence =
                0.0f;

            pair.second.entanglementStrength =
                0.0f;
        }
    }
}

void
NeuralQuantumEntanglementEngine::
decayQuantumStates(
    float deltaTime) {

    for(auto& pair :
        states_) {

        pair.second.coherence -=
            deltaTime * 0.00005f;

        if(pair.second.coherence <
            0.0f) {

            pair.second.coherence =
                0.0f;
        }

        pair.second.uncertainty +=
            deltaTime * 0.00001f;

        if(pair.second.uncertainty >
            1.0f) {

            pair.second.uncertainty =
                1.0f;
        }
    }
}

void
NeuralQuantumEntanglementEngine::
update(
    float deltaTime) {

    decayQuantumStates(
        deltaTime);

    computeGlobalCoherence();

    computeNeuralEntanglement();

    computeQuantumStability();

    computeTemporalIntegrity();

    computeConsciousnessCoupling();

    computePredictiveResonance();

    computeUncertaintyFactor();

    computeDecoherenceRisk();

    stabilizeQuantumField();

    collapseUnstableStates();
}

const QuantumSynchronizationFrame&
NeuralQuantumEntanglementEngine::
getFrame() const {

    return frame_;
}

const std::unordered_map<
    uint64_t,
    QuantumNeuralState>&
NeuralQuantumEntanglementEngine::
getStates() const {

    return states_;
}

}