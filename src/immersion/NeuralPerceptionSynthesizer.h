#pragma once

#include <array>
#include <vector>

namespace dnxr {

struct SyntheticPerceptionFrame {

    std::vector<float> visualSignals;

    std::vector<float> auditorySignals;

    std::vector<float> tactileSignals;

    std::vector<float> vestibularSignals;

    std::vector<float> thermalSignals;

    std::vector<float> proprioceptiveSignals;
};

class NeuralPerceptionSynthesizer {
public:

    NeuralPerceptionSynthesizer();

    void synthesizeVisualLayer(
        const std::vector<float>& visualData);

    void synthesizeAuditoryLayer(
        const std::vector<float>& audioData);

    void synthesizeTactileLayer(
        const std::vector<float>& tactileData);

    void synthesizeVestibularLayer(
        const std::array<float,3>& acceleration);

    void synthesizeThermalLayer(
        const std::vector<float>& thermalData);

    void synthesizeProprioceptionLayer(
        const std::vector<float>& skeletalData);

    SyntheticPerceptionFrame
    buildFrame() const;

private:

    SyntheticPerceptionFrame frame_;
};

}

namespace neuroxr {
using namespace ::dnxr;
}
