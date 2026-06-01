#include "../src/safety/SensoryExposureBudget.h"

#include <cassert>

int main() {
    dnxr::SensoryExposureBudget budget;
    std::array<float, static_cast<std::size_t>(dnxr::SensoryModality::Count)> load{};
    load[static_cast<std::size_t>(dnxr::SensoryModality::Vestibular)] = 0.2f;
    for (int i = 0; i < 3600; ++i) {
        budget.observe(load, 1.0 / 60.0);
    }
    const auto report = budget.lastReport();
    assert(report.sessionExposure[static_cast<std::size_t>(dnxr::SensoryModality::Vestibular)] > 10.0f);
    return 0;
}
