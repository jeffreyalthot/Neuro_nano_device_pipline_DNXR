#include "SensoryExposureBudget.h"

#include <algorithm>
#include <cmath>

namespace dnxr {
namespace {

constexpr std::size_t modalityCount = static_cast<std::size_t>(SensoryModality::Count);

float finitePositiveOr(float value, float fallback) {
    return std::isfinite(value) && value > 0.0f ? value : fallback;
}

} // namespace

SensoryExposureLimits::SensoryExposureLimits() {
    loadPerSecond = {1.8f, 1.5f, 1.2f, 0.45f, 0.75f, 0.45f};
    sessionExposure = {120.0f, 100.0f, 80.0f, 20.0f, 45.0f, 18.0f};
}

SensoryExposureBudget::SensoryExposureBudget(SensoryExposureLimits limits)
    : limits_(limits) {}

SensoryExposureReport SensoryExposureBudget::observe(
    const std::array<float, modalityCount>& modalityLoad,
    double deltaTimeSeconds) {

    std::lock_guard<std::mutex> lock(mutex_);
    const float dt = static_cast<float>(std::clamp(deltaTimeSeconds, 0.0, 1.0));
    report_.withinBudget = true;
    report_.severity = 0.0f;
    report_.reason = "sensory_exposure_nominal";

    vestibularCooldownRemainingSeconds_ = std::max(0.0, vestibularCooldownRemainingSeconds_ - dt);
    proprioceptiveCooldownRemainingSeconds_ = std::max(0.0, proprioceptiveCooldownRemainingSeconds_ - dt);

    for (std::size_t i = 0; i < modalityCount; ++i) {
        const float load = std::clamp(std::isfinite(modalityLoad[i]) ? modalityLoad[i] : 0.0f, 0.0f, 1.0f);
        const float tau = finitePositiveOr(limits_.smoothingTimeConstantSeconds, 1.0f);
        const float alpha = dt > 0.0f ? std::clamp(1.0f - std::exp(-dt / tau), 0.0f, 1.0f) : 0.0f;
        report_.loadPerSecond[i] = (report_.loadPerSecond[i] * (1.0f - alpha)) + (load * alpha);
        report_.sessionExposure[i] += load * dt;

        const float perSecondLimit = finitePositiveOr(limits_.loadPerSecond[i], 1.0f);
        const float sessionLimit = finitePositiveOr(limits_.sessionExposure[i], 1.0f);
        const float perSecondRatio = report_.loadPerSecond[i] / perSecondLimit;
        const float sessionRatio = report_.sessionExposure[i] / sessionLimit;
        const float ratio = std::max(perSecondRatio, sessionRatio);
        if (ratio > 1.0f) {
            report_.withinBudget = false;
            report_.severity = std::max(report_.severity, std::clamp(ratio - 1.0f, 0.0f, 1.0f));
            report_.reason = "sensory_exposure_budget_exceeded";
        }
    }

    const auto vestibular = static_cast<std::size_t>(SensoryModality::Vestibular);
    const auto proprioceptive = static_cast<std::size_t>(SensoryModality::Proprioceptive);
    const float vestibularInstantLoad = std::clamp(std::isfinite(modalityLoad[vestibular]) ? modalityLoad[vestibular] : 0.0f, 0.0f, 1.0f);
    const float proprioceptiveInstantLoad = std::clamp(std::isfinite(modalityLoad[proprioceptive]) ? modalityLoad[proprioceptive] : 0.0f, 0.0f, 1.0f);
    if (vestibularInstantLoad > finitePositiveOr(limits_.vestibularInstantPeakLimit, 0.35f)) {
        vestibularCooldownRemainingSeconds_ = std::max(
            vestibularCooldownRemainingSeconds_,
            static_cast<double>(limits_.vestibularCooldownSeconds));
    }
    if (proprioceptiveInstantLoad > finitePositiveOr(limits_.proprioceptiveInstantPeakLimit, 0.60f)) {
        proprioceptiveCooldownRemainingSeconds_ = std::max(
            proprioceptiveCooldownRemainingSeconds_,
            static_cast<double>(limits_.proprioceptiveCooldownSeconds));
    }

    report_.vestibularCooldownActive = vestibularCooldownRemainingSeconds_ > 0.0;
    report_.proprioceptiveCooldownActive = proprioceptiveCooldownRemainingSeconds_ > 0.0;
    if (report_.vestibularCooldownActive || report_.proprioceptiveCooldownActive) {
        report_.withinBudget = false;
        report_.reason = report_.vestibularCooldownActive
            ? "vestibular_exposure_cooldown"
            : "proprioceptive_exposure_cooldown";
        report_.severity = std::max(report_.severity, 0.25f);
    }

    return report_;
}

void SensoryExposureBudget::reset() {
    std::lock_guard<std::mutex> lock(mutex_);
    report_ = SensoryExposureReport{};
    vestibularCooldownRemainingSeconds_ = 0.0;
    proprioceptiveCooldownRemainingSeconds_ = 0.0;
}

void SensoryExposureBudget::setLimits(const SensoryExposureLimits& limits) {
    std::lock_guard<std::mutex> lock(mutex_);
    limits_ = limits;
}

SensoryExposureReport SensoryExposureBudget::lastReport() const {
    std::lock_guard<std::mutex> lock(mutex_);
    return report_;
}

} // namespace dnxr
