#pragma once

#include "FullDiveRuntimeInput.h"
#include "RuntimePipelineFrame.h"
#include "RuntimeStageTypes.h"
#include "../safety/SafetyStateMachine.h"
#include "../utils/SessionSafetyJournal.h"

#include <functional>
#include <string>
#include <vector>

namespace dnxr {

class RuntimeFullDiveStage {
public:
    struct Input {
        std::function<void(const FullDiveRuntimeInput&)> updateFromRuntime;
        std::function<FullDiveRuntimeSnapshot()> evaluateAndRemember;
        std::function<SafetyDecision(const FullDiveRuntimeSnapshot&, const SafetyDecision&)> mergeIntoDecision;
        std::function<void(const FullDiveRuntimeSnapshot&)> applyToSafetyStateMachine;
        std::function<SessionAuditEvent(const std::string&, const std::string&, float, const SessionAuditMetadata&)> recordSafetyEvent;
        std::function<void()> triggerEmergency;
        std::function<std::vector<float>(const std::vector<float>&)> runDecompression;
        RuntimeMetricsSnapshot runtimeMetrics;
    };

    static const char* name();
    static RuntimeStageStatus validate(bool healthy, const std::string& failureReason);
    static RuntimePipelineFrame execute(RuntimePipelineFrame frame, const Input& input);
};

} // namespace dnxr
