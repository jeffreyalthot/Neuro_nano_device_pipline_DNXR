#pragma once

#include <array>
#include <cstdint>
#include <vector>

namespace dnxr {

enum class OscillationBand {

    DELTA,
    THETA,
    ALPHA,
    BETA,
    GAMMA
};

struct OscillationSignal {

    OscillationBand band;

    float amplitude;

    float frequency;

    float phase;

    float synchronization;

    float corticalInfluence;
};

struct NeuralOscillationState {

    std::vector<OscillationSignal>
    signals;

    float globalSynchronization;

    float neuralNoise;

    float consciousnessStability;

    float immersionResonance;

    float predictiveResonance;
};

class NeuralOscillationModel {
public:

    NeuralOscillationModel();

    void initialize();

    void generateOscillationSignal(
        OscillationBand band,
        float amplitude,
        float frequency);

    void updateOscillation(
        float deltaTime);

    void synchronizeSignals();

    void computeGlobalSynchronization();

    void computeNeuralNoise();

    void computeConsciousnessStability();

    void computeImmersionResonance();

    void stabilizeOscillatoryNetwork();

    const NeuralOscillationState&
    getState() const;

private:

    NeuralOscillationState state_;
};

}

namespace neuroxr {
using OscillationBand = ::dnxr::OscillationBand;
using OscillationSignal = ::dnxr::OscillationSignal;
using NeuralOscillationState = ::dnxr::NeuralOscillationState;
using NeuralOscillationModel = ::dnxr::NeuralOscillationModel;
}
