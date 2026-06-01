#include "SimulatedSignalProvider.h"

#include <cmath>
#include <utility>

namespace dnxr {

SimulatedSignalProvider::SimulatedSignalProvider(std::vector<float> seedSignal)
    : seedSignal_(std::move(seedSignal)),
      simulator_() {
    if (seedSignal_.empty()) {
        seedSignal_ = {0.2f, 0.4f, 0.6f, 0.3f, 0.1f};
    }
}

SimulatedSignalProvider::SimulatedSignalProvider(NeuralSignalSimulatorConfig config)
    : simulator_(config),
      useScenarioSimulator_(true) {}

std::vector<float> SimulatedSignalProvider::nextSignal(int cycle) {
    if (useScenarioSimulator_) {
        return simulator_.sample(cycle < 0 ? 0U : static_cast<std::uint64_t>(cycle));
    }

    std::vector<float> signal = seedSignal_;
    const float phase = static_cast<float>(cycle) * 0.05f;
    for (std::size_t i = 0; i < signal.size(); ++i) {
        const float modulation = std::sin(phase + static_cast<float>(i) * 0.31f) * 0.015f;
        signal[i] += modulation;
    }
    return signal;
}

std::string SimulatedSignalProvider::name() const {
    return useScenarioSimulator_ ? "simulated:" + toString(simulator_.config().scenario) : "simulated";
}

} // namespace dnxr
