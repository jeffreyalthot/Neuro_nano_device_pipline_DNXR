#include "../src/safety/SensoryExposureBudget.h"

#include <cassert>

int main() {
    dnxr::SensoryExposureLimits limits;
    limits.loadPerSecond[static_cast<std::size_t>(dnxr::SensoryModality::Vestibular)] = 0.05f;
    limits.sessionExposure[static_cast<std::size_t>(dnxr::SensoryModality::Vestibular)] = 0.2f;
    limits.vestibularCooldownSeconds = 0.5f;

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
    return 0;
}
