#include "../src/safety/SensoryExposureBudget.h"

#include <cassert>

int main() {
    dnxr::SensoryExposureLimits limits;
    limits.loadPerSecond[static_cast<std::size_t>(dnxr::SensoryModality::Vestibular)] = 0.05f;
    limits.sessionExposure[static_cast<std::size_t>(dnxr::SensoryModality::Vestibular)] = 0.2f;
    limits.vestibularCooldownSeconds = 0.5f;

    limits.vestibularInstantPeakLimit = 0.35f;

    dnxr::SensoryExposureBudget budget(limits);
    std::array<float, static_cast<std::size_t>(dnxr::SensoryModality::Count)> load{};
    load[static_cast<std::size_t>(dnxr::SensoryModality::Vestibular)] = 0.5f;

    const auto first = budget.observe(load, 0.1);
    assert(!first.withinBudget);
    assert(first.vestibularCooldownActive);
    assert(first.reason == "vestibular_exposure_cooldown");

    load[static_cast<std::size_t>(dnxr::SensoryModality::Vestibular)] = 0.0f;
    budget.observe(load, 1.0);
    budget.observe(load, 1.0);
    const auto second = budget.observe(load, 1.0);
    assert(second.withinBudget);
    assert(!second.vestibularCooldownActive);

    dnxr::SensoryExposureLimits temporalLimits;
    temporalLimits.vestibularInstantPeakLimit = 1.0f;
    temporalLimits.loadPerSecond[static_cast<std::size_t>(dnxr::SensoryModality::Vestibular)] = 1.0f;
    temporalLimits.sessionExposure[static_cast<std::size_t>(dnxr::SensoryModality::Vestibular)] = 100.0f;

    dnxr::SensoryExposureBudget hz60(temporalLimits);
    dnxr::SensoryExposureBudget hz90(temporalLimits);
    dnxr::SensoryExposureBudget jitter(temporalLimits);
    load[static_cast<std::size_t>(dnxr::SensoryModality::Vestibular)] = 0.4f;

    for (int i = 0; i < 60; ++i) {
        hz60.observe(load, 1.0 / 60.0);
    }
    for (int i = 0; i < 90; ++i) {
        hz90.observe(load, 1.0 / 90.0);
    }
    for (int i = 0; i < 60; ++i) {
        jitter.observe(load, (i % 2 == 0) ? (1.0 / 45.0) : (1.0 / 90.0));
    }

    const auto r60 = hz60.lastReport();
    const auto r90 = hz90.lastReport();
    const float l60 = r60.loadPerSecond[static_cast<std::size_t>(dnxr::SensoryModality::Vestibular)];
    const float l90 = r90.loadPerSecond[static_cast<std::size_t>(dnxr::SensoryModality::Vestibular)];
    assert(l60 > 0.20f);
    assert(l90 > 0.20f);
    assert(l60 > l90 - 0.03f && l60 < l90 + 0.03f);
    assert(jitter.lastReport().sessionExposure[static_cast<std::size_t>(dnxr::SensoryModality::Vestibular)] > 0.0f);

    dnxr::SensoryExposureLimits freezeLimits;
    freezeLimits.vestibularInstantPeakLimit = 1.0f;
    freezeLimits.maxExposureAccountingSeconds = 10.0f;
    freezeLimits.sessionExposure[static_cast<std::size_t>(dnxr::SensoryModality::Vestibular)] = 100.0f;
    dnxr::SensoryExposureBudget freezeBudget(freezeLimits);
    freezeBudget.observe(load, 5.0);
    const auto freeze5 = freezeBudget.lastReport();
    assert(freeze5.sessionExposure[static_cast<std::size_t>(dnxr::SensoryModality::Vestibular)] > 1.9f);
    freezeBudget.observe(load, 30.0);
    const auto freeze30 = freezeBudget.lastReport();
    assert(freeze30.sessionExposure[static_cast<std::size_t>(dnxr::SensoryModality::Vestibular)] < 6.1f);
    assert(freeze30.loadPerSecond[static_cast<std::size_t>(dnxr::SensoryModality::Vestibular)] <= 0.4f);
    return 0;
}
