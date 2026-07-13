#include "RuntimeFullDiveStage.h"

#include "RuntimeStageServices.h"
#include "FullDiveSafetyBridge.h"
#include "../safety/CognitiveReversibilitySystem.h"
#include "../safety/EmergencyShutdown.h"

#include <algorithm>

namespace dnxr {

namespace {

void applySimulationReviewSafetyMetadata(RuntimePipelineFrame& frame) {
    frame.fullDiveNeuralFrame.sequenceId = frame.chipFrame.sequenceId;
    frame.fullDiveNeuralFrame.monotonicTimestampMs = static_cast<std::uint64_t>(std::max(0.0, frame.timestamp * 1000.0));
    frame.fullDiveNeuralFrame.safety.clinicalModeLocked = true;
    frame.fullDiveNeuralFrame.safety.painSimulationBlocked = true;
    frame.fullDiveNeuralFrame.safety.emergencyCutoffAvailable = frame.chipFrame.simulation && frame.chipFrame.signalQuality > 0.0f;
    frame.fullDiveNeuralFrame.safety.aggregateRisk = std::clamp(frame.fusedSafetyDecision.riskScore, 0.0f, 1.0f);
    frame.fullDiveNeuralFrame.safety.patientCalibrationHash = frame.fullDiveNeuralFrame.safety.patientCalibrationHash.empty()
        ? "signed_calibration_artifact_required"
        : frame.fullDiveNeuralFrame.safety.patientCalibrationHash;
}

} // namespace

const char* RuntimeFullDiveStage::name() {
    return "full_dive";
}

RuntimeStageStatus RuntimeFullDiveStage::validate(bool healthy, const std::string& failureReason) {
    return healthy ? RuntimeStageStatus::ok(name()) : RuntimeStageStatus::degraded(failureReason);
}

RuntimePipelineFrame RuntimeFullDiveStage::execute(RuntimePipelineFrame frame, const Input& input) {
    if (!frame.continueCycle) {
        return frame;
    }

    frame.fullDiveStatus = validate(
        input.updateFromRuntime && input.evaluateAndRemember && input.mergeIntoDecision,
        "full_dive_stage_disabled");
    if (!frame.fullDiveStatus.continueCycle) {
        return frame;
    }

    FullDiveRuntimeInput fullDiveInput;
    fullDiveInput.cognitive = frame.cognitive;
    fullDiveInput.biometric = frame.biometric;
    fullDiveInput.diagnostics = frame.diagnostics;
    fullDiveInput.calibrationDrift = frame.calibrationDrift;
    fullDiveInput.safetyDecision = frame.fusedSafetyDecision;
    fullDiveInput.realityAnchorPlan = frame.realityAnchorPlan;
    fullDiveInput.sensorySync = frame.sensorySync;
    fullDiveInput.runtimeMetrics = input.runtimeMetrics;
    fullDiveInput.deltaTimeSeconds = frame.deltaTimeSeconds;
    input.updateFromRuntime(fullDiveInput);

    const FullDiveRuntimeSnapshot snapshot = input.evaluateAndRemember();
    frame.fullDiveSnapshot = snapshot;
    frame.fusedSafetyDecision = input.mergeIntoDecision(snapshot, frame.fusedSafetyDecision);
    applySimulationReviewSafetyMetadata(frame);

    if (!snapshot.safetyInterventionRequired()) {
        frame.fullDiveStatus = validate(true, "full_dive_stage_unhealthy");
        return frame;
    }

    if (input.recordSafetyEvent) {
        SessionAuditMetadata metadata;
        metadata.runtimeCycle = frame.cycleId;
        metadata.safetyAction = snapshot.traceTag;
        metadata.renderAllowed = snapshot.intervention != FullDiveInterventionLevel::EmergencyStop &&
                                 snapshot.intervention != FullDiveInterventionLevel::Lockout;
        metadata.emergencyStop = snapshot.intervention == FullDiveInterventionLevel::EmergencyStop ||
                                 snapshot.intervention == FullDiveInterventionLevel::Lockout;
        input.recordSafetyEvent(
            "full_dive_intervention",
            snapshot.traceTag + "|" + snapshot.reason,
            snapshot.severity,
            metadata);
    }

    if (input.applyToSafetyStateMachine) {
        input.applyToSafetyStateMachine(snapshot);
    }

    if (snapshot.intervention == FullDiveInterventionLevel::EmergencyStop ||
        snapshot.intervention == FullDiveInterventionLevel::Lockout) {
        if (input.triggerEmergency) {
            input.triggerEmergency();
        }
        frame.continueCycle = false;
        frame.emergencyActive = true;
        frame.stopReason = snapshot.reason;
        frame.fullDiveStatus = RuntimeStageStatus::emergency(snapshot.reason);
        return frame;
    }

    if ((snapshot.intervention == FullDiveInterventionLevel::Decompress ||
         snapshot.intervention == FullDiveInterventionLevel::RealityAnchor) && input.runDecompression) {
        frame.sensoryFrame = input.runDecompression(frame.sensoryFrame);
    }

    if (snapshot.intervention == FullDiveInterventionLevel::ReduceIntensity) {
        const float scale = std::clamp(1.0f - snapshot.severity, 0.25f, 1.0f);
        for (float& channel : frame.sensoryFrame) {
            channel *= scale;
        }
    }

    frame.fullDiveStatus = validate(true, "full_dive_stage_unhealthy");
    return frame;
}

RuntimePipelineFrame RuntimeFullDiveStage::execute(
    RuntimePipelineFrame frame,
    RuntimeStageServices& services,
    const RuntimeStageContext& context,
    const RuntimeMetricsSnapshot& metricsSnapshot) {
    (void)context;
    Input input;
    input.updateFromRuntime = [&services](const FullDiveRuntimeInput& runtimeInput) {
        services.fullDiveSynchronization.updateFromRuntime(runtimeInput);
    };
    input.evaluateAndRemember = [&services]() {
        return services.fullDiveBridge.evaluateAndRemember(services.fullDiveSynchronization);
    };
    input.mergeIntoDecision = [&services](const FullDiveRuntimeSnapshot& snapshot, const SafetyDecision& decision) {
        return services.fullDiveBridge.mergeIntoDecision(snapshot, decision);
    };
    input.applyToSafetyStateMachine = [&services](const FullDiveRuntimeSnapshot& snapshot) {
        services.fullDiveBridge.apply(snapshot, services.safetyStateMachine);
    };
    input.recordSafetyEvent = [&services](const std::string& category, const std::string& message, float riskScore, const SessionAuditMetadata& metadata) {
        return services.safetyJournal.record(category, message, riskScore, metadata);
    };
    input.triggerEmergency = [&services]() { services.emergency.trigger(); };
    input.runDecompression = [&services](const std::vector<float>& sensoryFrame) {
        return services.reversibility.runDecompression(sensoryFrame);
    };
    input.runtimeMetrics = metricsSnapshot;
    return execute(std::move(frame), input);
}

} // namespace dnxr
