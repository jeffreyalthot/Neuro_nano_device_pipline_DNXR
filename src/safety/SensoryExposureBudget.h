#pragma once

#include <array>
#include <cstddef>
#include <mutex>
#include <string>

#include "../immersion/SensoryFrameSchema.h"

namespace dnxr {

struct SensoryExposureLimits {
    std::array<float, static_cast<std::size_t>(SensoryModality::Count)> loadPerSecond{};
    std::array<float, static_cast<std::size_t>(SensoryModality::Count)> sessionExposure{};
    float vestibularCooldownSeconds = 2.0f;
    float proprioceptiveCooldownSeconds = 1.0f;
    float vestibularInstantPeakLimit = 0.35f;
    float proprioceptiveInstantPeakLimit = 0.60f;
    float smoothingTimeConstantSeconds = 1.0f;
    float maxExposureAccountingSeconds = 10.0f;

    SensoryExposureLimits();
};

struct SensoryExposureReport {
    std::array<float, static_cast<std::size_t>(SensoryModality::Count)> loadPerSecond{};
    std::array<float, static_cast<std::size_t>(SensoryModality::Count)> sessionExposure{};
    bool withinBudget = true;
    bool vestibularCooldownActive = false;
    bool proprioceptiveCooldownActive = false;
    float severity = 0.0f;
    std::string reason = "sensory_exposure_nominal";
};

class SensoryExposureBudget {
public:
    explicit SensoryExposureBudget(SensoryExposureLimits limits = SensoryExposureLimits{});

    SensoryExposureReport observe(
        const std::array<float, static_cast<std::size_t>(SensoryModality::Count)>& modalityLoad,
        double deltaTimeSeconds);
    void reset();
    void setLimits(const SensoryExposureLimits& limits);
    SensoryExposureReport lastReport() const;

private:
    mutable std::mutex mutex_;
    SensoryExposureLimits limits_;
    SensoryExposureReport report_;
    double vestibularCooldownRemainingSeconds_ = 0.0;
    double proprioceptiveCooldownRemainingSeconds_ = 0.0;
};

} // namespace dnxr
