#pragma once

#include "RuntimePipelineFrame.h"
#include "RuntimeStageTypes.h"
#include "../safety/SafetyStateMachine.h"
#include "../utils/SessionSafetyJournal.h"

#include <functional>
#include <string>
#include <vector>

namespace dnxr {

class RuntimeSafetyStage {
public:
    struct Input {
        std::function<SafetyDecision(const SafetyGovernorInput&)> evaluateSafety;
        std::function<SessionAuditEvent(const std::string&, const std::string&, float, const SessionAuditMetadata&)> recordSafetyEvent;
        std::function<SafetyStateTransition(const SafetyDecision&)> observeSafetyDecision;
        std::function<RealityAnchorPlan(const RealityAnchorContext&, const SafetyDecision&)> evaluateRealityAnchor;
        std::function<SafetyStateTransition(const RealityAnchorPlan&)> observeRealityAnchorPlan;
        std::function<std::vector<float>(const std::vector<float>&, const SafetyDecision&)> applySafetyToFrame;
        std::function<std::vector<float>(const std::vector<float>&, const RealityAnchorPlan&)> applyAnchorToFrame;
        std::function<std::vector<float>(const std::vector<float>&, float)> optimizeThermalFrame;
        std::function<bool(const std::vector<float>&)> shouldStartReturnToReality;
        std::function<std::vector<float>(const std::vector<float>&)> runDecompression;
        std::function<void()> triggerEmergency;
        std::function<void(const std::string&)> logWarn;
        std::function<void(const std::string&)> logError;
        double sessionElapsedSeconds = 0.0;
        bool emergencyStable = true;
    };

    static const char* name();
    static RuntimeStageStatus validate(bool healthy, const std::string& failureReason);
    static RuntimePipelineFrame execute(RuntimePipelineFrame frame, const Input& input);
};

} // namespace dnxr
