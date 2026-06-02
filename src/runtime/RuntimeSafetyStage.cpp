#include "RuntimeSafetyStage.h"

#include <algorithm>

namespace dnxr {

const char* RuntimeSafetyStage::name() {
    return "safety";
}

RuntimeStageStatus RuntimeSafetyStage::validate(bool healthy, const std::string& failureReason) {
    return healthy ? RuntimeStageStatus::ok(name()) : RuntimeStageStatus::degraded(failureReason);
}

RuntimePipelineFrame RuntimeSafetyStage::execute(RuntimePipelineFrame frame, const Input& input) {
    if (!frame.continueCycle) {
        return frame;
    }

    frame.safetyStatus = validate(
        input.evaluateSafety && input.evaluateRealityAnchor && input.applySafetyToFrame && input.applyAnchorToFrame,
        "safety_stage_unavailable");
    if (!frame.safetyStatus.continueCycle) {
        frame.continueCycle = false;
        frame.stopReason = frame.safetyStatus.reason;
        return frame;
    }

    const SafetyDecision safetyDecision = input.evaluateSafety({
        frame.normalizedTelemetryRisk,
        frame.sensoryFrame,
        frame.symbolicState,
        frame.cognitive,
        frame.biometric,
        frame.diagnostics
    });
    frame.safetyDecision = safetyDecision;
    frame.fusedSafetyDecision = safetyDecision;

    SessionAuditEvent safetyAudit{};
    if (input.recordSafetyEvent) {
        safetyAudit = input.recordSafetyEvent(
            "safety_decision",
            safetyDecision.reason + "|" + safetyDecision.auditRecord,
            safetyDecision.riskScore,
            SessionAuditMetadata{frame.cycleId, safetyDecision.reason, safetyDecision.allowRender, safetyDecision.requiresEmergencyStop});
    }

    if (input.observeSafetyDecision) {
        const SafetyStateTransition transition = input.observeSafetyDecision(safetyDecision);
        if (transition.latched) {
            if (input.triggerEmergency) {
                input.triggerEmergency();
            }
            frame.continueCycle = false;
            frame.emergencyActive = true;
            frame.stopReason = "safety_state_machine:" + transition.reason;
            frame.safetyStatus = RuntimeStageStatus::emergency(frame.stopReason);
            return frame;
        }
    }

    if (safetyDecision.requiresEmergencyStop || !safetyDecision.allowRender) {
        if (input.triggerEmergency) {
            input.triggerEmergency();
        }
        if (input.logError) {
            input.logError("[SafetyGovernor] Emergency stop before render | " +
                           safetyDecision.reason + " | " + safetyDecision.auditRecord +
                           " | journal=" + safetyAudit.eventHash);
        }
        frame.continueCycle = false;
        frame.emergencyActive = true;
        frame.stopReason = "safety_governor:" + safetyDecision.reason;
        frame.safetyStatus = RuntimeStageStatus::emergency(frame.stopReason);
        return frame;
    }

    RealityAnchorContext anchorContext;
    anchorContext.userAlias = "runtime_user";
    anchorContext.sessionId = "runtime_session";
    anchorContext.realEnvironmentLabel = "simulation_lab";
    anchorContext.sessionElapsedSeconds = input.sessionElapsedSeconds;
    anchorContext.consentActive = frame.diagnostics.autonomyPreservation >= 0.20f;
    anchorContext.emergencyActive = !input.emergencyStable;
    anchorContext.identityContinuity = frame.diagnostics.autonomyPreservation;
    anchorContext.temporalOrientation = frame.cognitive.attention;
    anchorContext.realWorldOrientation = frame.biometric.postureStability;
    anchorContext.sensoryOverload = std::max(
        std::max(
            std::clamp(frame.cognitive.stress + frame.cognitive.fatigue, 0.0f, 1.0f),
            frame.sensorySync.degradationApplied ? 1.0f - frame.sensorySync.recommendedIntensityScale : 0.0f),
        frame.calibrationDrift.blockCriticalActions ? frame.calibrationDrift.driftScore : 0.0f);

    const RealityAnchorPlan anchorPlan = input.evaluateRealityAnchor(anchorContext, safetyDecision);
    frame.realityAnchorPlan = anchorPlan;

    if (input.observeRealityAnchorPlan) {
        const SafetyStateTransition transition = input.observeRealityAnchorPlan(anchorPlan);
        if (transition.latched) {
            if (input.triggerEmergency) {
                input.triggerEmergency();
            }
            if (input.recordSafetyEvent) {
                input.recordSafetyEvent(
                    "reality_anchor_state_transition",
                    transition.reason,
                    anchorPlan.riskScore,
                    SessionAuditMetadata{frame.cycleId, anchorPlan.traceTag, false, true});
            }
            frame.continueCycle = false;
            frame.emergencyActive = true;
            frame.stopReason = "safety_state_machine:" + transition.reason;
            frame.safetyStatus = RuntimeStageStatus::emergency(frame.stopReason);
            return frame;
        }
    }

    if (anchorPlan.requiresEmergencyStop || !anchorPlan.allowRender) {
        if (input.triggerEmergency) {
            input.triggerEmergency();
        }
        SessionAuditEvent anchorAudit{};
        if (input.recordSafetyEvent) {
            anchorAudit = input.recordSafetyEvent(
                "reality_anchor_stop",
                anchorPlan.traceTag + "|" + anchorPlan.recoveryAction,
                anchorPlan.riskScore,
                SessionAuditMetadata{frame.cycleId, anchorPlan.traceTag, false, true});
        }
        if (input.logError) {
            input.logError("[RealityAnchorSystem] Emergency anchor before render | " +
                           anchorPlan.traceTag + " | journal=" + anchorAudit.eventHash);
        }
        frame.continueCycle = false;
        frame.emergencyActive = true;
        frame.stopReason = anchorPlan.traceTag;
        frame.safetyStatus = RuntimeStageStatus::emergency(frame.stopReason);
        return frame;
    }

    frame.sensoryFrame = input.applySafetyToFrame(frame.sensoryFrame, safetyDecision);
    frame.sensoryFrame = input.applyAnchorToFrame(frame.sensoryFrame, anchorPlan);
    if (safetyDecision.action != SafetyAction::Continue || anchorPlan.mode != RealityAnchorMode::Maintain) {
        if (input.logWarn) {
            input.logWarn("[SafetyGovernor] action=" + safetyDecision.reason +
                          " risk=" + std::to_string(safetyDecision.riskScore) +
                          " scale=" + std::to_string(safetyDecision.intensityScale) +
                          " anchor=" + anchorPlan.traceTag);
        }
    }

    if (input.optimizeThermalFrame) {
        frame.sensoryFrame = input.optimizeThermalFrame(frame.sensoryFrame, frame.cognitive.immersionIndex);
    }

    const bool shouldDecompress = safetyDecision.action == SafetyAction::Decompress ||
        safetyDecision.action == SafetyAction::RealityAnchor ||
        anchorPlan.mode == RealityAnchorMode::RealityAnchor ||
        (input.shouldStartReturnToReality && input.shouldStartReturnToReality(frame.normalizedTelemetryRisk));
    if (shouldDecompress && input.runDecompression) {
        frame.sensoryFrame = input.runDecompression(frame.sensoryFrame);
        if (input.logWarn) {
            input.logWarn("[CognitiveReversibility] Return-to-reality decompression applied. anchor=" +
                          anchorPlan.traceTag);
        }
    }

    frame.safetyStatus = validate(true, "safety_stage_unhealthy");
    return frame;
}

} // namespace dnxr
