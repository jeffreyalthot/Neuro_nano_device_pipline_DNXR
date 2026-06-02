#include "../src/runtime/RuntimeCycleFinalizer.h"

#include <cassert>
#include <atomic>
#include <mutex>

int main() {
    dnxr::NeuroRealtimeScheduler scheduler;
    dnxr::RuntimeMetrics metrics;
    dnxr::RuntimeTraceRecorder traces;
    std::mutex metricsMutex;
    std::mutex traceMutex;
    std::atomic<std::uint64_t> cycleCounter{5};
    std::atomic<std::uint64_t> droppedFrames{0};
    bool published = false;

    dnxr::SchedulerTickReport report = scheduler.beginTick();
    const auto snapshot = dnxr::RuntimeCycleFinalizer::finish(
        report,
        true,
        false,
        "test_nominal",
        dnxr::RuntimeCycleFinalizer::Input{
            scheduler,
            metrics,
            traces,
            metricsMutex,
            traceMutex,
            cycleCounter,
            droppedFrames,
            [&published](const dnxr::RuntimeMetricsSnapshot&) { published = true; }
        },
        true);

    assert(published);
    assert(snapshot.totalCycles == 1);
    assert(cycleCounter.load() == 6);
    assert(!traces.events().empty());
    return 0;
}
