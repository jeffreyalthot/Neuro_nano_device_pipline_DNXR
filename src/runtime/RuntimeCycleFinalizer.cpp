#include "RuntimeCycleFinalizer.h"

namespace dnxr {
namespace {

template <typename PublishRuntimeMetricsCallback, typename LogWarnCallback>
RuntimeMetricsSnapshot finishWithCallbacks(
    SchedulerTickReport& tickReport,
    NeuroRealtimeScheduler& scheduler,
    RuntimeMetrics& metrics,
    RuntimeTraceRecorder& traceRecorder,
    std::mutex& metricsMutex,
    std::mutex& traceMutex,
    std::atomic<std::uint64_t>& cycleCounter,
    std::atomic<std::uint64_t>& droppedFrames,
    PublishRuntimeMetricsCallback&& publishRuntimeMetrics,
    LogWarnCallback&& logWarn,
    bool safeDecision,
    bool emergencyActive,
    const std::string& reason,
    bool emitBudgetWarning) {
    scheduler.endTick(tickReport);

    RuntimeMetricsSnapshot snapshot;
    {
        std::lock_guard<std::mutex> lock(metricsMutex);
        metrics.recordCycleMeasurement(tickReport.totalMs, safeDecision, emergencyActive, reason);
        snapshot = metrics.snapshot();
    }
    {
        std::lock_guard<std::mutex> traceLock(traceMutex);
        traceRecorder.record(
            cycleCounter.load(),
            "RuntimeController",
            safeDecision ? "cycle_complete" : "cycle_degraded",
            tickReport.totalMs,
            emergencyActive ? 1.0f : 0.0f,
            !safeDecision || emergencyActive,
            reason);
    }

    if (publishRuntimeMetrics) {
        publishRuntimeMetrics(snapshot);
    }

    if (emitBudgetWarning && !tickReport.withinBudget) {
        ++droppedFrames;
        if (logWarn) {
            logWarn("⚠️ Runtime frame budget exceeded total_ms="
                    + std::to_string(tickReport.totalMs)
                    + " jitter_ms=" + std::to_string(tickReport.jitterMs)
                    + " over_budget_tasks=" + std::to_string(tickReport.overBudgetTaskCount)
                    + " p99_ms=" + std::to_string(snapshot.p99LatencyMs)
                    + " latency_health=" + runtimeLatencyHealthToString(snapshot.latencyHealth));
        }
    }

    ++cycleCounter;
    return snapshot;
}

} // namespace

RuntimeMetricsSnapshot RuntimeCycleFinalizer::finish(
    SchedulerTickReport& tickReport,
    bool safeDecision,
    bool emergencyActive,
    const std::string& reason,
    const Input& input,
    bool emitBudgetWarning) {
    return finishWithCallbacks(
        tickReport,
        input.scheduler,
        input.metrics,
        input.traceRecorder,
        input.metricsMutex,
        input.traceMutex,
        input.cycleCounter,
        input.droppedFrames,
        input.publishRuntimeMetrics,
        input.logWarn,
        safeDecision,
        emergencyActive,
        reason,
        emitBudgetWarning);
}

RuntimeMetricsSnapshot RuntimeCycleFinalizer::finish(
    SchedulerTickReport& tickReport,
    const RuntimePipelineFrame& frame,
    RuntimeStageServices& services,
    RuntimePipelineSinks& sinks,
    const Context& context,
    const Options& options) {
    (void)frame;
    (void)services;
    return finishWithCallbacks(
        tickReport,
        context.scheduler,
        context.metrics,
        context.traceRecorder,
        context.metricsMutex,
        context.traceMutex,
        context.cycleCounter,
        context.droppedFrames,
        sinks.publishRuntimeMetrics,
        sinks.logWarn,
        options.safeDecision,
        options.emergencyActive,
        options.reason,
        options.emitBudgetWarning);
}

RuntimeMetricsSnapshot RuntimeCycleFinalizer::finish(
    SchedulerTickReport& tickReport,
    const RuntimePipelineFrame& frame,
    RuntimeStageServices& services,
    RuntimePipelineSinkRefs sinks,
    const Context& context,
    const Options& options) {
    (void)frame;
    (void)services;
    return finishWithCallbacks(
        tickReport,
        context.scheduler,
        context.metrics,
        context.traceRecorder,
        context.metricsMutex,
        context.traceMutex,
        context.cycleCounter,
        context.droppedFrames,
        sinks.publishRuntimeMetrics,
        sinks.logWarn,
        options.safeDecision,
        options.emergencyActive,
        options.reason,
        options.emitBudgetWarning);
}

} // namespace dnxr
