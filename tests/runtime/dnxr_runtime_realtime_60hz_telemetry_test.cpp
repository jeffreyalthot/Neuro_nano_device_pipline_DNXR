#include "../../src/runtime/CanonicalNeuralBciPipeline.h"
#include "../../src/runtime/RuntimeRealtimeTelemetry.h"

#include <cassert>
#include <cmath>

int main() {
    const auto rate = dnxr::RuntimeRateContract::negotiate(60.0, 90.0);
    assert(rate.hardwareCompatible);
    assert(std::fabs(rate.frameBudgetMs - 16.6666667) < 0.001);

    const auto slower = dnxr::RuntimeRateContract::negotiate(60.0, 40.0);
    assert(!slower.hardwareCompatible);
    assert(slower.negotiatedHz == 40.0);
    assert(slower.frameBudgetMs == 25.0);

    dnxr::RuntimeRealtimeTelemetry telemetry(rate);
    const auto t0 = dnxr::RuntimeRealtimeTelemetry::clock::now();
    telemetry.beginCycle(t0);
    telemetry.recordStage(dnxr::CanonicalBciStage::AcquireWorld, 0.20);
    telemetry.recordStage(dnxr::CanonicalBciStage::AcquireWorld, 0.30);
    telemetry.recordStage(dnxr::CanonicalBciStage::DispatchTransport, 0.40);
    telemetry.recordQueueDepths(1U, 0U);
    telemetry.recordDroppedWorldFrame("world_backpressure_keep_latest");
    telemetry.recordHeldChipFrame("ack_timeout_no_stale_stimulus");
    telemetry.endCycle(t0 + std::chrono::microseconds(16000));

    const auto snapshot = telemetry.snapshot();
    assert(snapshot.cycles == 1U);
    assert(snapshot.lastCycleMs > 15.0 && snapshot.lastCycleMs < 17.0);
    assert(snapshot.droppedWorldFrames == 1U);
    assert(snapshot.heldChipFrames == 1U);
    assert(snapshot.worldQueueDepth == 1U);
    const auto acquireWorldIndex = static_cast<std::size_t>(dnxr::CanonicalBciStage::AcquireWorld);
    assert(snapshot.stages[acquireWorldIndex].sampleCount == 2U);
    assert(snapshot.stages[acquireWorldIndex].p50Ms == 0.20);
    assert(snapshot.stages[acquireWorldIndex].p95Ms == 0.30);
    assert(snapshot.stages[acquireWorldIndex].wcetMs == 0.30);

    dnxr::RuntimeBoundedLatestQueue<int, 2U> queue;
    assert(queue.pushLatest(1));
    assert(queue.pushLatest(2));
    assert(!queue.pushLatest(3));
    assert(queue.droppedCount() == 1U);
    int value = 0;
    assert(queue.pop(value));
    assert(value == 2 || value == 3);
    assert(queue.pop(value));
    assert(value == 3);
    assert(!queue.pop(value));
    return 0;
}
