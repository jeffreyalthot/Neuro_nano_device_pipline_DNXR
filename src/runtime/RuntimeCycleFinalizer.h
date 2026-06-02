#pragma once

#include "../core/NeuroRealtimeScheduler.h"
#include "../utils/RuntimeMetrics.h"
#include "../utils/RuntimeTraceRecorder.h"

#include <atomic>
#include <cstdint>
#include <functional>
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
        std::function<void(const RuntimeMetricsSnapshot&)> publishRuntimeMetrics;
        std::function<void(const std::string&)> logWarn;
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
