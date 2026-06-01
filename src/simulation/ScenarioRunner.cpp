#include "ScenarioRunner.h"

namespace dnxr {

ScenarioRunner::ScenarioRunner(NeuralSignalSimulatorConfig config)
    : signalSimulator_(config),
      biometricSimulator_(config.scenario) {}

SimulationFrame ScenarioRunner::nextFrame() {
    return frameAt(cycle_++);
}

SimulationFrame ScenarioRunner::frameAt(std::uint64_t cycle) const {
    SimulationFrame frame;
    frame.cycle = cycle;
    frame.scenarioName = toString(signalSimulator_.config().scenario);
    frame.neuralSignal = signalSimulator_.sample(cycle);
    frame.biometrics = biometricSimulator_.sample(cycle);
    frame.dropoutActive = signalSimulator_.config().scenario == SimulationScenario::SignalDropout &&
                          signalSimulator_.config().dropoutEveryCycles > 0U &&
                          cycle > 0U &&
                          cycle % signalSimulator_.config().dropoutEveryCycles == 0U;
    frame.safetyStressExpected = frame.biometrics.heartRateBpm > 120.0f ||
                                 frame.biometrics.respiratoryRateRpm > 28.0f ||
                                 frame.biometrics.focusIndex < 0.4f;
    return frame;
}

void ScenarioRunner::reset() {
    cycle_ = 0U;
}

void ScenarioRunner::setScenario(SimulationScenario scenario) {
    signalSimulator_.setScenario(scenario);
    biometricSimulator_.setScenario(scenario);
}

SimulationScenario ScenarioRunner::scenario() const {
    return signalSimulator_.config().scenario;
}

std::uint64_t ScenarioRunner::cycle() const {
    return cycle_;
}

} // namespace dnxr
