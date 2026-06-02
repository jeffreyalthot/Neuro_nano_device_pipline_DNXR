#include "RuntimeCycleFinalizer.h"

namespace dnxr {

RuntimeMetricsSnapshot RuntimeCycleFinalizer::finish(
    SchedulerTickReport& tickReport,
    bool safeDecision,
    bool emergencyActive,
    const std::string& reason,
    const Input& input,
    bool emitBudgetWarning) {
    input.scheduler.endTick(tickReport);

    RuntimeMetricsSnapshot snapshot;
    {
        std::lock_guard<std::mutex> lock(input.metricsMutex);
        input.metrics.recordCycleMeasurement(tickReport.totalMs, safeDecision, emergencyActive, reason);
        snapshot = input.metrics.snapshot();
    }
    {
        std::lock_guard<std::mutex> traceLock(input.traceMutex);
        input.traceRecorder.record(
            input.cycleCounter.load(),
            "RuntimeController",
            safeDecision ? "cycle_complete" : "cycle_degraded",
            tickReport.totalMs,
            emergencyActive ? 1.0f : 0.0f,
            !safeDecision || emergencyActive,
            reason);
    }

    if (input.publishRuntimeMetrics) {
        input.publishRuntimeMetrics(snapshot);
    }

    if (emitBudgetWarning && !tickReport.withinBudget) {
        ++input.droppedFrames;
        if (input.logWarn) {
            input.logWarn("⚠️ Runtime frame budget exceeded total_ms="
                          + std::to_string(tickReport.totalMs)
                          + " jitter_ms=" + std::to_string(tickReport.jitterMs)
                          + " over_budget_tasks=" + std::to_string(tickReport.overBudgetTaskCount)
                          + " p99_ms=" + std::to_string(snapshot.p99LatencyMs)
                          + " latency_health=" + runtimeLatencyHealthToString(snapshot.latencyHealth));
        }
    }

    ++input.cycleCounter;
    return snapshot;
}

} // namespace dnxr
