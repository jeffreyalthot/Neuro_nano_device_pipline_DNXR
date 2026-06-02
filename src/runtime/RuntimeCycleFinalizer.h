#pragma once

#include "RuntimeCallback.h"
#include "../core/NeuroRealtimeScheduler.h"
#include "../utils/RuntimeMetrics.h"
#include "../utils/RuntimeTraceRecorder.h"

#include <atomic>
#include <cstdint>
#include <mutex>
#include <string>

namespace dnxr {

class RuntimeCycleFinalizer {
public:
    struct Input {
        NeuroRealtimeScheduler& scheduler;
        RuntimeMetrics& metrics;
        RuntimeTraceRecorder& traceRecorder;
        std::mutex& metricsMutex;
        std::mutex& traceMutex;
        std::atomic<std::uint64_t>& cycleCounter;
        std::atomic<std::uint64_t>& droppedFrames;
        RuntimeCallback<void(const RuntimeMetricsSnapshot&)> publishRuntimeMetrics;
        RuntimeCallback<void(const std::string&)> logWarn;
    };

    static RuntimeMetricsSnapshot finish(
        SchedulerTickReport& tickReport,
        bool safeDecision,
        bool emergencyActive,
        const std::string& reason,
        const Input& input,
        bool emitBudgetWarning);
};

} // namespace dnxr
