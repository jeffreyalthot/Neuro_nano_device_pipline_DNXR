#pragma once

#include "RuntimePipelineFrame.h"
#include "RuntimeStageTypes.h"
#include "../safety/IndependentWatchdog.h"
#include "../safety/SafetyStateMachine.h"

#include <functional>
#include <string>
#include <vector>

namespace dnxr {

class RuntimeRenderStage {
public:
    struct Input {
        std::function<void(const std::vector<float>&)> present;
        std::function<bool()> finishRenderBudget;
        std::function<void()> onRenderBudgetExceeded;
        std::function<void()> heartbeatRenderer;
        std::function<WatchdogAssessment(double, bool, float, float)> assessWatchdog;
        std::function<void()> triggerEmergency;
        std::function<void(const std::string&)> latchEmergency;
        std::function<void(const std::vector<float>&)> pushSensoryFrame;
        std::function<void(const std::string&)> logWarn;
        std::function<void(const std::string&)> logError;
        double runtimeLatencyMs = 0.0;
    };

    static const char* name();
    static RuntimeStageStatus validate(bool healthy, const std::string& failureReason);
    static RuntimePipelineFrame execute(RuntimePipelineFrame frame, const Input& input);
};

} // namespace dnxr
