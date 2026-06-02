#include "../src/core/RuntimeController.h"
#include "../src/utils/Logger.h"

#include <cassert>
#include <cstdint>
#include <string>

int main() {
    dnxr::Logger::clearSinks();
    dnxr::Logger::setMinLevel(dnxr::LogLevel::Fatal);

    dnxr::RuntimeController controller;
    controller.setFrameBudgetMs(50);

    std::uint64_t stateEvents = 0U;
    std::uint64_t metricsEvents = 0U;
    controller.setRuntimeEventCallback([&](const std::string& payload) {
        if (payload.rfind("state:", 0) == 0) {
            ++stateEvents;
        }
        if (payload.rfind("metrics:", 0) == 0) {
            ++metricsEvents;
        }
    });

    constexpr std::uint64_t kCycles = 8U;
    for (std::uint64_t i = 0; i < kCycles; ++i) {
        const auto frame = controller.runDeterministicCycle(4000.0 + static_cast<double>(i) * 0.011);
        assert(frame.continueCycle);
        assert(!frame.emergencyActive);
        assert(frame.stopReason == "nominal_runtime");
        assert(!frame.sensoryFrame.empty());
    }

    assert(controller.getCycleCount() == kCycles);
    assert(stateEvents == kCycles);
    assert(metricsEvents == kCycles);
    controller.stop();
    return 0;
}
