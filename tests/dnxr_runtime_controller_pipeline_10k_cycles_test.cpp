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

    constexpr std::uint64_t kCycles = 10000U;
    std::uint64_t stateEvents = 0U;
    controller.setRuntimeEventCallback([&stateEvents](const std::string& payload) {
        if (payload.rfind("state:", 0) == 0) {
            ++stateEvents;
        }
    });

    for (std::uint64_t i = 0; i < kCycles; ++i) {
        const dnxr::RuntimePipelineFrame frame = controller.runDeterministicCycle(2000.0 + static_cast<double>(i) * 0.011);
        assert(frame.cycleId == i);
        assert(!frame.emergencyActive);
        assert(frame.stopReason == "nominal_runtime");
    }

    assert(controller.getCycleCount() == kCycles);
    assert(stateEvents == kCycles);
    controller.stop();
    return 0;
}
