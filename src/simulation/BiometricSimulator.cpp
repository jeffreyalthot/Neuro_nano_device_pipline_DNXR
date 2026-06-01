#include "BiometricSimulator.h"

#include <algorithm>
#include <cmath>

namespace dnxr {

std::vector<float> BiometricSample::toTelemetryVector() const {
    return {heartRateBpm, respiratoryRateRpm, focusIndex, neuralNoise, oxygenLevel, eyeStability};
}

BiometricSimulator::BiometricSimulator(SimulationScenario scenario)
    : scenario_(scenario) {}

BiometricSample BiometricSimulator::sample(std::uint64_t cycle) const {
    const float t = static_cast<float>(cycle);
    BiometricSample sample;
    sample.heartRateBpm = 72.0f + std::sin(t * 0.05f) * 3.0f;
    sample.respiratoryRateRpm = 16.0f + std::cos(t * 0.04f) * 1.5f;
    sample.focusIndex = 0.86f + std::cos(t * 0.03f) * 0.04f;
    sample.neuralNoise = 0.02f + std::abs(std::sin(t * 0.08f)) * 0.01f;
    sample.oxygenLevel = 0.98f;
    sample.eyeStability = 0.91f + std::sin(t * 0.025f) * 0.02f;

    switch (scenario_) {
        case SimulationScenario::FatigueRise:
            sample.heartRateBpm -= std::min(10.0f, t * 0.03f);
            sample.respiratoryRateRpm -= std::min(3.0f, t * 0.01f);
            sample.focusIndex -= std::min(0.45f, t * 0.002f);
            break;
        case SimulationScenario::StressSpike:
            if (cycle % 24U >= 10U && cycle % 24U <= 14U) {
                sample.heartRateBpm += 34.0f;
                sample.respiratoryRateRpm += 7.0f;
                sample.focusIndex -= 0.22f;
            }
            break;
        case SimulationScenario::SignalDropout:
            if (cycle > 0U && cycle % 16U == 0U) {
                sample.neuralNoise = 0.0f;
                sample.eyeStability = 0.0f;
            }
            break;
        case SimulationScenario::NoisySensor:
            sample.neuralNoise += 0.16f;
            sample.eyeStability -= 0.22f;
            break;
        case SimulationScenario::HighIntentAmbiguity:
            sample.focusIndex = 0.55f + std::sin(t * 0.2f) * 0.2f;
            break;
        case SimulationScenario::DissociationRisk:
            sample.focusIndex = 0.34f;
            sample.eyeStability = 0.42f;
            break;
        case SimulationScenario::AddictionRiskPattern:
            sample.focusIndex = (cycle % 12U < 3U) ? 0.98f : 0.62f;
            sample.heartRateBpm += (cycle % 12U < 3U) ? 12.0f : 0.0f;
            break;
        case SimulationScenario::LatencySpike:
            sample.neuralNoise += (cycle % 30U == 0U) ? 0.08f : 0.0f;
            break;
        case SimulationScenario::RenderOverload:
            sample.eyeStability -= 0.18f;
            sample.focusIndex -= 0.10f;
            break;
        case SimulationScenario::NominalFocus:
            break;
    }

    sample.heartRateBpm = std::max(35.0f, std::min(160.0f, sample.heartRateBpm));
    sample.respiratoryRateRpm = std::max(4.0f, std::min(36.0f, sample.respiratoryRateRpm));
    sample.focusIndex = std::max(0.0f, std::min(1.0f, sample.focusIndex));
    sample.neuralNoise = std::max(0.0f, std::min(1.0f, sample.neuralNoise));
    sample.oxygenLevel = std::max(0.0f, std::min(1.0f, sample.oxygenLevel));
    sample.eyeStability = std::max(0.0f, std::min(1.0f, sample.eyeStability));
    return sample;
}

void BiometricSimulator::setScenario(SimulationScenario scenario) {
    scenario_ = scenario;
}

SimulationScenario BiometricSimulator::scenario() const {
    return scenario_;
}

} // namespace dnxr
