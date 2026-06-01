#pragma once

#include "IInputSignalProvider.h"
#include "../simulation/NeuralSignalSimulator.h"

namespace dnxr {

class SimulatedSignalProvider final : public IInputSignalProvider {
public:
    explicit SimulatedSignalProvider(std::vector<float> seedSignal = {0.2f, 0.4f, 0.6f, 0.3f, 0.1f});
    explicit SimulatedSignalProvider(NeuralSignalSimulatorConfig config);

    std::vector<float> nextSignal(int cycle) override;
    std::string name() const override;

private:
    std::vector<float> seedSignal_;
    NeuralSignalSimulator simulator_;
    bool useScenarioSimulator_ = false;
};

} // namespace dnxr
