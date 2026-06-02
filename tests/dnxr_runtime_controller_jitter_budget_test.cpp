#include "../src/core/RuntimeController.h"
#include "../src/utils/Logger.h"

#include <cassert>
#include <cstdint>

int main() {
    dnxr::Logger::clearSinks();
    dnxr::Logger::setMinLevel(dnxr::LogLevel::Fatal);

    dnxr::RuntimeController controller;
    controller.setFrameBudgetMs(50);

    constexpr std::uint64_t kCycles = 64U;
    for (std::uint64_t i = 0; i < kCycles; ++i) {
        const auto frame = controller.runDeterministicCycle(4000.0 + static_cast<double>(i) * 0.011111);
        assert(!frame.emergencyActive);
        assert(frame.stopReason == "nominal_runtime");
    }

    const dnxr::RuntimeMetricsSnapshot metrics = controller.getRuntimeMetricsSnapshot();
    assert(metrics.totalCycles == kCycles);
    assert(metrics.jitterMs >= 0.0);
    assert(metrics.p95LatencyMs >= 0.0);
    assert(metrics.p99LatencyMs >= metrics.p95LatencyMs);
    assert(metrics.latencyBudgetExceededCount <= kCycles);
    controller.stop();
    return 0;
}
