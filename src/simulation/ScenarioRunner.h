#pragma once

#include "BiometricSimulator.h"
#include "NeuralSignalSimulator.h"

#include <cstdint>
#include <string>

namespace dnxr {

struct SimulationFrame {
    std::uint64_t cycle = 0;
    std::string scenarioName;
    std::vector<float> neuralSignal;
    BiometricSample biometrics;
    bool dropoutActive = false;
    bool safetyStressExpected = false;
};

class ScenarioRunner {
public:
    explicit ScenarioRunner(NeuralSignalSimulatorConfig config = {});

    SimulationFrame nextFrame();
    SimulationFrame frameAt(std::uint64_t cycle) const;
    void reset();
    void setScenario(SimulationScenario scenario);
    SimulationScenario scenario() const;
    std::uint64_t cycle() const;

private:
    NeuralSignalSimulator signalSimulator_;
    BiometricSimulator biometricSimulator_;
    std::uint64_t cycle_ = 0U;
};

} // namespace dnxr
