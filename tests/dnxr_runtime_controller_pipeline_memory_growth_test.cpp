#include "../src/core/RuntimeController.h"
#include "../src/utils/Logger.h"

#include <cassert>
#include <cstdint>
#include <sys/resource.h>

namespace {
long residentSetKb() {
    rusage usage{};
    getrusage(RUSAGE_SELF, &usage);
    return usage.ru_maxrss;
}
}

int main() {
    dnxr::Logger::clearSinks();
    dnxr::Logger::setMinLevel(dnxr::LogLevel::Fatal);

    dnxr::RuntimeController controller;
    controller.setFrameBudgetMs(50);

    constexpr std::uint64_t kWarmup = 32U;
    constexpr std::uint64_t kMeasuredCycles = 64U;
    for (std::uint64_t i = 0; i < kWarmup; ++i) {
        const auto frame = controller.runDeterministicCycle(2000.0 + static_cast<double>(i) * 0.011);
        assert(!frame.emergencyActive);
    }

    const long beforeKb = residentSetKb();
    for (std::uint64_t i = 0; i < kMeasuredCycles; ++i) {
        const auto frame = controller.runDeterministicCycle(3000.0 + static_cast<double>(i) * 0.011);
        assert(!frame.emergencyActive);
    }
    const long afterKb = residentSetKb();

    assert(controller.getCycleCount() == kWarmup + kMeasuredCycles);
    assert(afterKb >= beforeKb);
    assert((afterKb - beforeKb) < 8192L);
    controller.stop();
    return 0;
}
