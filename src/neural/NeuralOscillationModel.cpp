#include "NeuralOscillationModel.h"

#include <cmath>

namespace dnxr {

NeuralOscillationModel::
NeuralOscillationModel() {

    initialize();
}

void NeuralOscillationModel::
initialize() {

    state_.signals.clear();

    state_.globalSynchronization = 1.0f;

    state_.neuralNoise = 0.0f;

    state_.consciousnessStability = 1.0f;

    state_.immersionResonance = 0.0f;

    state_.predictiveResonance = 0.0f;
}

void NeuralOscillationModel::
generateOscillationSignal(
    OscillationBand band,
    float amplitude,
    float frequency) {

    OscillationSignal signal;

    signal.band = band;

    signal.amplitude = amplitude;

    signal.frequency = frequency;

    signal.phase = 0.0f;

    signal.synchronization = 1.0f;

    signal.corticalInfluence =
        amplitude * frequency;

    state_.signals.push_back(signal);
}

void NeuralOscillationModel::
updateOscillation(
    float deltaTime) {

    for(auto& signal :
        state_.signals) {

        signal.phase +=
            signal.frequency *
            deltaTime;

        signal.amplitude =
            std::sin(signal.phase);
    }
}

void NeuralOscillationModel::
synchronizeSignals() {

    for(auto& signal :
        state_.signals) {

        signal.synchronization =
            std::cos(signal.phase) *
            0.5f +
            0.5f;
    }
}

void NeuralOscillationModel::
computeGlobalSynchronization() {

    float sync = 0.0f;

    for(const auto& signal :
        state_.signals) {

        sync +=
            signal.synchronization;
    }

    if(!state_.signals.empty()) {

        sync /=
            static_cast<float>(
                state_.signals.size());
    }

    state_.globalSynchronization =
        sync;
}

void NeuralOscillationModel::
computeNeuralNoise() {

    float noise = 0.0f;

    for(const auto& signal :
        state_.signals) {

        noise +=
            std::abs(signal.amplitude);
    }

    if(!state_.signals.empty()) {

        noise /=
            static_cast<float>(
                state_.signals.size());
    }

    state_.neuralNoise =
        1.0f - noise;
}

void NeuralOscillationModel::
computeConsciousnessStability() {

    state_.consciousnessStability =
        (state_.globalSynchronization * 0.7f) +
        ((1.0f -
        state_.neuralNoise) * 0.3f);
}

void NeuralOscillationModel::
computeImmersionResonance() {

    state_.immersionResonance =
        state_.consciousnessStability *
        state_.globalSynchronization;
}

void NeuralOscillationModel::
stabilizeOscillatoryNetwork() {

    state_.predictiveResonance =
        state_.immersionResonance *
        0.85f;
}

const NeuralOscillationState&
NeuralOscillationModel::
getState() const {

    return state_;
}

}