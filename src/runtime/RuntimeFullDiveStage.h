#pragma once

#include "FullDiveRuntimeInput.h"
#include "RuntimePipelineFrame.h"
#include "RuntimeCallback.h"
#include "RuntimeStageTypes.h"
#include "RuntimeStageContext.h"
#include "../safety/SafetyStateMachine.h"
#include "../utils/SessionSafetyJournal.h"

#include <string>
#include <vector>

namespace dnxr {

class RuntimeStageServices;

class RuntimeFullDiveStage {
public:
    struct Input {
        RuntimeCallback<void(const FullDiveRuntimeInput&)> updateFromRuntime;
        RuntimeCallback<FullDiveRuntimeSnapshot()> evaluateAndRemember;
        RuntimeCallback<SafetyDecision(const FullDiveRuntimeSnapshot&, const SafetyDecision&)> mergeIntoDecision;
        RuntimeCallback<void(const FullDiveRuntimeSnapshot&)> applyToSafetyStateMachine;
        RuntimeCallback<SessionAuditEvent(const std::string&, const std::string&, float, const SessionAuditMetadata&)> recordSafetyEvent;
        RuntimeCallback<void()> triggerEmergency;
        RuntimeCallback<std::vector<float>(const std::vector<float>&)> runDecompression;
        RuntimeMetricsSnapshot runtimeMetrics;
    };

    static const char* name();
    static RuntimeStageStatus validate(bool healthy, const std::string& failureReason);
    static RuntimePipelineFrame execute(RuntimePipelineFrame frame, const Input& input);
    static RuntimePipelineFrame execute(RuntimePipelineFrame frame, RuntimeStageServices& services, const RuntimeStageContext& context, const RuntimeMetricsSnapshot& metricsSnapshot);
};

} // namespace dnxr
