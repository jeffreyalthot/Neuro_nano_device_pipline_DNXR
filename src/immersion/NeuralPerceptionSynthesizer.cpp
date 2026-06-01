#include "NeuralPerceptionSynthesizer.h"

#include <cmath>

namespace dnxr {

NeuralPerceptionSynthesizer::
NeuralPerceptionSynthesizer() {
}

void NeuralPerceptionSynthesizer::
synthesizeVisualLayer(
    const std::vector<float>& visualData) {

    frame_.visualSignals.clear();

    for(float value : visualData) {

        float encoded =
            std::sin(value * 3.141592f);

        frame_.visualSignals.push_back(encoded);
    }
}

void NeuralPerceptionSynthesizer::
synthesizeAuditoryLayer(
    const std::vector<float>& audioData) {

    frame_.auditorySignals.clear();

    for(float value : audioData) {

        float encoded =
            std::cos(value * 1.618f);

        frame_.auditorySignals.push_back(encoded);
    }
}

void NeuralPerceptionSynthesizer::
synthesizeTactileLayer(
    const std::vector<float>& tactileData) {

    frame_.tactileSignals.clear();

    for(float value : tactileData) {

        frame_.tactileSignals.push_back(
            value * 0.75f);
    }
}

void NeuralPerceptionSynthesizer::
synthesizeVestibularLayer(
    const std::array<float,3>& acceleration) {

    frame_.vestibularSignals = {
        acceleration[0],
        acceleration[1],
        acceleration[2]
    };
}

void NeuralPerceptionSynthesizer::
synthesizeThermalLayer(
    const std::vector<float>& thermalData) {

    frame_.thermalSignals.clear();

    for(float value : thermalData) {

        frame_.thermalSignals.push_back(
            value * 0.5f);
    }
}

void NeuralPerceptionSynthesizer::
synthesizeProprioceptionLayer(
    const std::vector<float>& skeletalData) {

    frame_.proprioceptiveSignals.clear();

    for(float value : skeletalData) {

        frame_.proprioceptiveSignals.push_back(
            std::tanh(value));
    }
}

SyntheticPerceptionFrame
NeuralPerceptionSynthesizer::buildFrame() const {

    return frame_;
}

}