#pragma once

#include "RuntimePipelineFrame.h"
#include "RuntimeCallback.h"
#include "RuntimeStageTypes.h"
#include "../core/NeuroRealtimeScheduler.h"
#include "../safety/IndependentWatchdog.h"
#include "../safety/SafetyStateMachine.h"

#include <string>
#include <vector>

namespace dnxr {

class RuntimeStageServices;

class RuntimeRenderStage {
public:
    struct Input {
        RuntimeCallback<bool(const std::vector<float>&)> prepareNeuralSensory;
        RuntimeCallback<void(const std::vector<float>&)> publishObserverMirror;
        RuntimeCallback<bool()> finishRenderBudget;
        RuntimeCallback<void()> onRenderBudgetExceeded;
        RuntimeCallback<void()> heartbeatObserver;
        RuntimeCallback<WatchdogAssessment(double, bool, float, float)> assessWatchdog;
        RuntimeCallback<void()> triggerEmergency;
        RuntimeCallback<void(const std::string&)> latchEmergency;
        RuntimeCallback<void(const std::string&)> logWarn;
        RuntimeCallback<void(const std::string&)> logError;
        double runtimeLatencyMs = 0.0;
    };

    static const char* name();
    static RuntimeStageStatus validate(bool healthy, const std::string& failureReason);
    static RuntimePipelineFrame execute(RuntimePipelineFrame frame, const Input& input);
    static RuntimePipelineFrame execute(RuntimePipelineFrame frame, RuntimeStageServices& services, SchedulerTickReport& tickReport);
};

} // namespace dnxr
