#include "../src/safety/SensoryExposureBudget.h"

#include <cassert>
#include <thread>
#include <vector>

int main() {
    dnxr::SensoryExposureBudget budget;
    std::array<float, static_cast<std::size_t>(dnxr::SensoryModality::Count)> load{};
    load[static_cast<std::size_t>(dnxr::SensoryModality::Vestibular)] = 0.01f;
    load[static_cast<std::size_t>(dnxr::SensoryModality::Proprioceptive)] = 0.01f;

    std::vector<std::thread> workers;
    for (int worker = 0; worker < 8; ++worker) {
        workers.emplace_back([&budget, load]() {
            for (int i = 0; i < 128; ++i) {
                const auto report = budget.observe(load, 1.0 / 90.0);
                assert(!report.reason.empty());
            }
        });
    }

    for (auto& worker : workers) {
        worker.join();
    }

    const auto report = budget.lastReport();
    assert(report.sessionExposure[static_cast<std::size_t>(dnxr::SensoryModality::Vestibular)] > 0.0f);
    return 0;
}
