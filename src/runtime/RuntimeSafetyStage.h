#pragma once

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

class RuntimeSafetyStage {
public:
    struct Input {
        RuntimeCallback<SafetyDecision(const SafetyGovernorInput&)> evaluateSafety;
        RuntimeCallback<SessionAuditEvent(const std::string&, const std::string&, float, const SessionAuditMetadata&)> recordSafetyEvent;
        RuntimeCallback<SafetyStateTransition(const SafetyDecision&)> observeSafetyDecision;
        RuntimeCallback<RealityAnchorPlan(const RealityAnchorContext&, const SafetyDecision&)> evaluateRealityAnchor;
        RuntimeCallback<SafetyStateTransition(const RealityAnchorPlan&)> observeRealityAnchorPlan;
        RuntimeCallback<std::vector<float>(const std::vector<float>&, const SafetyDecision&)> applySafetyToFrame;
        RuntimeCallback<std::vector<float>(const std::vector<float>&, const RealityAnchorPlan&)> applyAnchorToFrame;
        RuntimeCallback<std::vector<float>(const std::vector<float>&, float)> optimizeThermalFrame;
        RuntimeCallback<bool(const std::vector<float>&)> shouldStartReturnToReality;
        RuntimeCallback<std::vector<float>(const std::vector<float>&)> runDecompression;
        RuntimeCallback<void()> triggerEmergency;
        RuntimeCallback<void(const std::string&)> logWarn;
        RuntimeCallback<void(const std::string&)> logError;
        double sessionElapsedSeconds = 0.0;
        bool emergencyStable = true;
    };

    static const char* name();
    static RuntimeStageStatus validate(bool healthy, const std::string& failureReason);
    static RuntimePipelineFrame execute(RuntimePipelineFrame frame, const Input& input);
    static RuntimePipelineFrame execute(RuntimePipelineFrame frame, RuntimeStageServices& services, const RuntimeStageContext& context);
};

} // namespace dnxr
