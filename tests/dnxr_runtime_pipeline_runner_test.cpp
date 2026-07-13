#include "../src/core/RuntimeController.h"
#include "../src/runtime/CanonicalNeuralBciPipeline.h"
#include "../src/utils/Logger.h"

#include <cassert>
#include <cstdint>

int main() {
    dnxr::Logger::clearSinks();
    dnxr::Logger::setMinLevel(dnxr::LogLevel::Fatal);

    assert(!dnxr::CanonicalNeuralBciPipeline::stageOrderIsCanonical({}));

    dnxr::RuntimeController controller;
    controller.setFrameBudgetMs(50);

    const std::uint64_t activeBefore = dnxr::CanonicalNeuralBciPipeline::activeInstanceCount();
    const auto frame = controller.runDeterministicCycle(42.0);
    assert(frame.runtimePathDecision.path == dnxr::RuntimePathKind::CanonicalAdapter);
    assert(!frame.stopReason.empty());
    assert(controller.getCanonicalPipelineMetrics().cyclesStarted == 1U);
    assert(controller.getCanonicalPipelineMetrics().cyclesCompleted == 1U);
    assert(controller.getCanonicalPipelineMetrics().delegatedCompatibilityCycles == 0U);
    assert(dnxr::CanonicalNeuralBciPipeline::activeInstanceCount() >= activeBefore);

    controller.stop();
    return 0;
}
