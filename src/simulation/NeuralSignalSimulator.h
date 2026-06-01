#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace dnxr {

enum class SimulationScenario {
    NominalFocus,
    FatigueRise,
    StressSpike,
    SignalDropout,
    NoisySensor,
    HighIntentAmbiguity,
    DissociationRisk,
    AddictionRiskPattern,
    LatencySpike,
    RenderOverload
};

std::string toString(SimulationScenario scenario);
SimulationScenario parseSimulationScenario(const std::string& value);

struct NeuralSignalSimulatorConfig {
    SimulationScenario scenario = SimulationScenario::NominalFocus;
    std::uint32_t channelCount = 5U;
    float baseAmplitude = 0.35f;
    float noiseAmplitude = 0.02f;
    std::uint64_t dropoutEveryCycles = 0U;
};

class NeuralSignalSimulator {
public:
    explicit NeuralSignalSimulator(NeuralSignalSimulatorConfig config = {});

    std::vector<float> sample(std::uint64_t cycle) const;
    const NeuralSignalSimulatorConfig& config() const;
    void setScenario(SimulationScenario scenario);

private:
    float deterministicNoise(std::uint64_t cycle, std::uint32_t channel) const;
    float scenarioEnvelope(std::uint64_t cycle, std::uint32_t channel) const;

    NeuralSignalSimulatorConfig config_;
};

} // namespace dnxr
