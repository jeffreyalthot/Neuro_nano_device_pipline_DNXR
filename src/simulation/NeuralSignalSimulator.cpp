#include "NeuralSignalSimulator.h"

#include <algorithm>
#include <cmath>

namespace dnxr {
namespace {

float clampSignal(float value) {
    return std::max(-1.0f, std::min(1.0f, value));
}

} // namespace

std::string toString(SimulationScenario scenario) {
    switch (scenario) {
        case SimulationScenario::NominalFocus: return "nominal_focus";
        case SimulationScenario::FatigueRise: return "fatigue_rise";
        case SimulationScenario::StressSpike: return "stress_spike";
        case SimulationScenario::SignalDropout: return "signal_dropout";
        case SimulationScenario::NoisySensor: return "noisy_sensor";
        case SimulationScenario::HighIntentAmbiguity: return "high_intent_ambiguity";
        case SimulationScenario::DissociationRisk: return "dissociation_risk";
        case SimulationScenario::AddictionRiskPattern: return "addiction_risk_pattern";
        case SimulationScenario::LatencySpike: return "latency_spike";
        case SimulationScenario::RenderOverload: return "render_overload";
    }
    return "nominal_focus";
}

SimulationScenario parseSimulationScenario(const std::string& value) {
    if (value == "fatigue_rise") return SimulationScenario::FatigueRise;
    if (value == "stress_spike") return SimulationScenario::StressSpike;
    if (value == "signal_dropout") return SimulationScenario::SignalDropout;
    if (value == "noisy_sensor") return SimulationScenario::NoisySensor;
    if (value == "high_intent_ambiguity") return SimulationScenario::HighIntentAmbiguity;
    if (value == "dissociation_risk") return SimulationScenario::DissociationRisk;
    if (value == "addiction_risk_pattern") return SimulationScenario::AddictionRiskPattern;
    if (value == "latency_spike") return SimulationScenario::LatencySpike;
    if (value == "render_overload") return SimulationScenario::RenderOverload;
    return SimulationScenario::NominalFocus;
}

NeuralSignalSimulator::NeuralSignalSimulator(NeuralSignalSimulatorConfig config)
    : config_(config) {
    if (config_.channelCount == 0U) {
        config_.channelCount = 5U;
    }
}

std::vector<float> NeuralSignalSimulator::sample(std::uint64_t cycle) const {
    if (config_.scenario == SimulationScenario::SignalDropout &&
        config_.dropoutEveryCycles > 0U &&
        cycle > 0U &&
        cycle % config_.dropoutEveryCycles == 0U) {
        return std::vector<float>(config_.channelCount, 0.0f);
    }

    std::vector<float> signal;
    signal.reserve(config_.channelCount);
    for (std::uint32_t channel = 0U; channel < config_.channelCount; ++channel) {
        const float phase = static_cast<float>(cycle) * 0.07f + static_cast<float>(channel) * 0.41f;
        const float carrier = std::sin(phase) * config_.baseAmplitude;
        const float envelope = scenarioEnvelope(cycle, channel);
        const float noise = deterministicNoise(cycle, channel);
        signal.push_back(clampSignal(carrier + envelope + noise));
    }
    return signal;
}

const NeuralSignalSimulatorConfig& NeuralSignalSimulator::config() const {
    return config_;
}

void NeuralSignalSimulator::setScenario(SimulationScenario scenario) {
    config_.scenario = scenario;
}

float NeuralSignalSimulator::deterministicNoise(std::uint64_t cycle, std::uint32_t channel) const {
    float amplitude = config_.noiseAmplitude;
    if (config_.scenario == SimulationScenario::NoisySensor) {
        amplitude *= 6.0f;
    }
    const auto seed = static_cast<float>((cycle + 1U) * (channel + 3U));
    return std::sin(seed * 12.9898f) * amplitude;
}

float NeuralSignalSimulator::scenarioEnvelope(std::uint64_t cycle, std::uint32_t channel) const {
    const float t = static_cast<float>(cycle);
    const float c = static_cast<float>(channel + 1U);
    switch (config_.scenario) {
        case SimulationScenario::NominalFocus:
            return 0.08f + 0.02f * std::cos(t * 0.03f + c);
        case SimulationScenario::FatigueRise:
            return -0.05f - std::min(0.35f, t * 0.002f) + 0.01f * c;
        case SimulationScenario::StressSpike:
            return (cycle % 24U >= 10U && cycle % 24U <= 14U) ? 0.55f : 0.05f;
        case SimulationScenario::SignalDropout:
            return 0.04f;
        case SimulationScenario::NoisySensor:
            return 0.02f * std::sin(t * 0.11f + c);
        case SimulationScenario::HighIntentAmbiguity:
            return (channel % 2U == 0U ? 0.28f : -0.28f) * std::sin(t * 0.09f);
        case SimulationScenario::DissociationRisk:
            return 0.42f * std::sin(t * 0.015f) - 0.12f;
        case SimulationScenario::AddictionRiskPattern:
            return (cycle % 12U < 3U) ? 0.62f : -0.04f;
        case SimulationScenario::LatencySpike:
            return (cycle % 30U == 0U) ? 0.48f : 0.03f;
        case SimulationScenario::RenderOverload:
            return 0.35f + 0.12f * std::sin(t * 0.2f + c);
    }
    return 0.0f;
}

} // namespace dnxr
