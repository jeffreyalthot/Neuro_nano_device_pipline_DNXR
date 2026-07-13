#include "../src/core/RuntimeController.h"
#include "../src/runtime/CanonicalNeuralBciPipeline.h"
#include "../src/utils/Logger.h"

#include <cassert>
#include <cstdint>
#include <string>

int main() {
    dnxr::Logger::clearSinks();
    dnxr::Logger::setMinLevel(dnxr::LogLevel::Fatal);

    const std::uint64_t activeBefore = dnxr::CanonicalNeuralBciPipeline::activeInstanceCount();
    {
        dnxr::RuntimeController controller;
        controller.setFrameBudgetMs(50);
        assert(dnxr::CanonicalNeuralBciPipeline::activeInstanceCount() == activeBefore + 1U);

        std::uint64_t metricsEvents = 0U;
        controller.setRuntimeEventCallback([&](const std::string& payload) {
            if (payload.rfind("metrics:", 0) == 0) {
                ++metricsEvents;
            }
        });

        constexpr std::uint64_t kCycles = 3U;
        for (std::uint64_t i = 0; i < kCycles; ++i) {
            const dnxr::RuntimePipelineFrame frame = controller.runDeterministicCycle(7000.0 + static_cast<double>(i) * 0.011);
            assert(frame.cycleId == i);
            assert(frame.runtimePathDecision.path == dnxr::RuntimePathKind::CanonicalAdapter);
            assert(!frame.runtimePathDecision.adapterFallbackAttempted);
            assert(!frame.emergencyActive);
        }

        const dnxr::CanonicalPipelineMetrics canonical = controller.getCanonicalPipelineMetrics();
        assert(canonical.cyclesStarted == kCycles);
        assert(canonical.cyclesCompleted == kCycles);
        assert(canonical.delegatedCompatibilityCycles == kCycles);
        assert(canonical.commitsPrepared == kCycles);
        assert(canonical.dispatchesObserved == kCycles);
        assert(canonical.metricRecords == kCycles);
        assert(canonical.rejectedDoubleStarts == 0U);
        assert(canonical.rejectedFrameIdRegressions == 0U);
        assert(canonical.rejectedAdapterFallbacks == 0U);
        assert(metricsEvents == kCycles);
        controller.stop();
    }
    assert(dnxr::CanonicalNeuralBciPipeline::activeInstanceCount() == activeBefore);
    return 0;
}
