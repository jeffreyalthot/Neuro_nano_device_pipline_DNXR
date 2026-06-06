#include "NeuralDoseLimiter.h"

#include <cmath>

namespace dnxr {
namespace { std::size_t idx(NeuralDoseModality m) { return static_cast<std::size_t>(m); } }

NeuralDoseLimiter::NeuralDoseLimiter(NeuralDoseLimits limits) : limits_(limits) {}

NeuralDoseDecision NeuralDoseLimiter::evaluateAndAccumulate(const std::vector<NeuralDoseSample>& samples) {
    NeuralDoseDecision decision;
    std::array<double, 8> proposed = sessionDose_;
    for (const auto& sample : samples) {
        const auto i = idx(sample.modality);
        if (!std::isfinite(sample.intensity) || !std::isfinite(sample.durationSeconds) || sample.intensity < 0.0 || sample.durationSeconds < 0.0) {
            decision.violations.push_back("invalid_dose_sample");
            continue;
        }
        const double dose = sample.intensity * sample.durationSeconds;
        if (sample.intensity > limits_.perSecond[i]) decision.violations.push_back("per_second_limit_exceeded");
        if (dose > limits_.perMinute[i]) decision.violations.push_back("per_minute_limit_exceeded");
        proposed[i] += dose;
        if (proposed[i] > limits_.perSession[i]) decision.violations.push_back("per_session_limit_exceeded");
    }
    decision.allowed = decision.violations.empty();
    if (decision.allowed) sessionDose_ = proposed;
    return decision;
}

const std::array<double, 8>& NeuralDoseLimiter::sessionDose() const { return sessionDose_; }

} // namespace dnxr
