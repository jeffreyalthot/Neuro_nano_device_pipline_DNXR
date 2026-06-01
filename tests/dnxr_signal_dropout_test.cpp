#include "../src/simulation/BiometricSimulator.h"
#include "../src/simulation/NeuralSignalSimulator.h"
#include "../src/simulation/ScenarioRunner.h"

#include <cassert>
#include <cmath>
#include <vector>

namespace {

bool allFinite(const std::vector<float>& values) {
    for (float value : values) {
        if (!std::isfinite(value)) {
            return false;
        }
    }
    return true;
}

bool allZero(const std::vector<float>& values) {
    for (float value : values) {
        if (value != 0.0f) {
            return false;
        }
    }
    return true;
}

} // namespace

int main() {
    dnxr::NeuralSignalSimulatorConfig dropoutConfig;
    dropoutConfig.scenario = dnxr::SimulationScenario::SignalDropout;
    dropoutConfig.channelCount = 6U;
    dropoutConfig.dropoutEveryCycles = 4U;

    dnxr::NeuralSignalSimulator dropoutSimulator(dropoutConfig);
    assert(dnxr::toString(dropoutSimulator.config().scenario) == "signal_dropout");
    assert(dnxr::parseSimulationScenario("stress_spike") == dnxr::SimulationScenario::StressSpike);

    const auto nominalFrame = dropoutSimulator.sample(1U);
    assert(nominalFrame.size() == 6U);
    assert(allFinite(nominalFrame));
    assert(!allZero(nominalFrame));

    const auto dropoutFrame = dropoutSimulator.sample(4U);
    assert(dropoutFrame.size() == 6U);
    assert(allFinite(dropoutFrame));
    assert(allZero(dropoutFrame));

    dnxr::BiometricSimulator stressBiometrics(dnxr::SimulationScenario::StressSpike);
    const auto stressSample = stressBiometrics.sample(12U);
    assert(stressSample.heartRateBpm > 100.0f);
    assert(stressSample.respiratoryRateRpm > 20.0f);
    assert(stressSample.toTelemetryVector().size() == 6U);
    assert(allFinite(stressSample.toTelemetryVector()));

    dnxr::ScenarioRunner runner(dropoutConfig);
    const auto runnerFrame0 = runner.nextFrame();
    const auto runnerFrame1 = runner.nextFrame();
    assert(runnerFrame0.cycle == 0U);
    assert(runnerFrame1.cycle == 1U);
    assert(runner.cycle() == 2U);
    assert(runnerFrame0.scenarioName == "signal_dropout");
    assert(!runnerFrame1.dropoutActive);

    runner.setScenario(dnxr::SimulationScenario::DissociationRisk);
    const auto riskFrame = runner.frameAt(10U);
    assert(riskFrame.scenarioName == "dissociation_risk");
    assert(riskFrame.safetyStressExpected);

    runner.reset();
    assert(runner.cycle() == 0U);

    return 0;
}
