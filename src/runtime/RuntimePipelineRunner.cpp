#include "RuntimePipelineRunner.h"
#include "RuntimeBodyPoseExtractionStage.h"
#include "RuntimeModalityPhaseLockStage.h"
#include "RuntimePostCorrectionSafetyStage.h"

#include <algorithm>
#include <chrono>
#include <utility>

namespace dnxr {

std::vector<float> RuntimePipelineRunner::normalizedTelemetryRiskVector(const std::vector<float>& telemetry) {
    if (telemetry.empty()) {
        return {};
    }

    const float heartRate = telemetry.size() > 0 ? telemetry[0] : 72.0f;
    const float stress = telemetry.size() > 1 ? telemetry[1] : 0.0f;
    const float focus = telemetry.size() > 2 ? telemetry[2] : 1.0f;
    const float neuralNoise = telemetry.size() > 3 ? telemetry[3] : 0.0f;
    const float oxygen = telemetry.size() > 4 ? telemetry[4] : 1.0f;
    const float eyeStability = telemetry.size() > 5 ? telemetry[5] : 1.0f;

    return {
        std::clamp((heartRate - 60.0f) / 80.0f, 0.0f, 1.0f),
        std::clamp(stress, 0.0f, 1.0f),
        std::clamp(1.0f - focus, 0.0f, 1.0f),
        std::clamp(neuralNoise * 8.0f, 0.0f, 1.0f),
        std::clamp(1.0f - oxygen, 0.0f, 1.0f),
        std::clamp(1.0f - eyeStability, 0.0f, 1.0f)
    };
}

void RuntimePipelineRunner::finishModernCycle(RuntimePipelineFrame& frame,
                                              SchedulerTickReport& tickReport,
                                              RuntimeStageServices& services,
                                              RuntimePipelineSinkRefs sinks,
                                              const RuntimeCycleFinalizer::Context& finalizer,
                                              bool safeDecision,
                                              bool emergencyActive,
                                              const std::string& reason,
                                              bool emitBudgetWarning) {
    RuntimeCycleFinalizer::finish(
        tickReport,
        frame,
        services,
        sinks,
        finalizer,
        RuntimeCycleFinalizer::Options{safeDecision, emergencyActive, reason, emitBudgetWarning});
    frame.continueCycle = false;
    frame.emergencyActive = emergencyActive;
    frame.stopReason = reason;
}

RuntimePipelineFrame RuntimePipelineRunner::run(const RuntimePipelineRunInput& input) const {
    RuntimePipelineRunInputRef refInput{
        input.context,
        input.tickReport,
        input.services,
        RuntimePipelineSinkRefs::fromSinks(input.sinks),
        input.runtimeMetricsSnapshot,
        input.finalizer};
    return run(refInput);
}

RuntimePipelineFrame RuntimePipelineRunner::run(const RuntimePipelineRunInputRef& input) const {
    RuntimePipelineFrame pipeline;

    const auto tAcq = std::chrono::steady_clock::now();
    pipeline = RuntimeAcquisitionStage::execute(input.context, input.services);
    input.finalizer.scheduler.endTask(RuntimeAcquisitionStage::name(), tAcq, input.tickReport);
    if (!pipeline.continueCycle) {
        if (pipeline.emergencyActive) {
            if (input.sinks.triggerEmergency) input.sinks.triggerEmergency();
            if (input.sinks.logWarn) input.sinks.logWarn("⚠️  Physiological guard triggered.");
            finishModernCycle(pipeline, input.tickReport, input.services, input.sinks, input.finalizer, false, true, pipeline.stopReason);
            if (input.sinks.throttleEmergency) input.sinks.throttleEmergency();
            return pipeline;
        }
        if (input.sinks.logWarn) input.sinks.logWarn("⚠️  Empty neural signal frame.");
        finishModernCycle(pipeline, input.tickReport, input.services, input.sinks, input.finalizer, false, pipeline.acquisitionStatus.emergencyActive, pipeline.stopReason);
        return pipeline;
    }

    const auto tSignal = std::chrono::steady_clock::now();
    pipeline = RuntimeSignalStage::execute(std::move(pipeline), input.services);
    input.finalizer.scheduler.endTask(RuntimeSignalStage::name(), tSignal, input.tickReport);
    if (!pipeline.continueCycle) {
        finishModernCycle(pipeline, input.tickReport, input.services, input.sinks, input.finalizer, false, pipeline.signalStatus.emergencyActive, pipeline.stopReason);
        return pipeline;
    }

    pipeline.normalizedTelemetryRisk = normalizedTelemetryRiskVector(pipeline.telemetry);

    const auto tInference = std::chrono::steady_clock::now();
    pipeline = RuntimeInferenceStage::execute(std::move(pipeline), input.services);
    const bool inferenceOk = input.finalizer.scheduler.endTask(RuntimeInferenceStage::name(), tInference, input.tickReport);
    if (!inferenceOk) {
        if (input.sinks.recordDroppedFrame) input.sinks.recordDroppedFrame();
        if (input.sinks.logWarn) input.sinks.logWarn("⚠️ Inference budget exceeded — frame degraded, skipping render. task=inference");
        finishModernCycle(pipeline, input.tickReport, input.services, input.sinks, input.finalizer, true, false, "inference_budget_exceeded");
        return pipeline;
    }

    if (input.sinks.publishPipelineState) input.sinks.publishPipelineState(pipeline);

    pipeline = RuntimeCalibrationStage::execute(std::move(pipeline), input.services);
    if (!pipeline.continueCycle) {
        finishModernCycle(pipeline, input.tickReport, input.services, input.sinks, input.finalizer, false, pipeline.calibrationStatus.emergencyActive, pipeline.stopReason);
        return pipeline;
    }

    pipeline = RuntimeImmersionFrameStage::execute(std::move(pipeline), input.services);
    if (!pipeline.continueCycle) {
        finishModernCycle(pipeline, input.tickReport, input.services, input.sinks, input.finalizer, false, pipeline.immersionFrameStatus.emergencyActive, pipeline.stopReason);
        return pipeline;
    }

    if (input.sinks.recordReplayFrame) input.sinks.recordReplayFrame(pipeline.cleanSignals, input.context.timestamp);

    pipeline.acquisitionChipFrame = pipeline.chipFrame;

    pipeline = RuntimeFullDiveStage::execute(std::move(pipeline), input.services, input.context, input.runtimeMetricsSnapshot);
    if (!pipeline.continueCycle) {
        finishModernCycle(pipeline, input.tickReport, input.services, input.sinks, input.finalizer, false, pipeline.emergencyActive, pipeline.stopReason);
        return pipeline;
    }

    pipeline = RuntimeFiveSenseCoherenceStage::execute(std::move(pipeline));
    if (!pipeline.continueCycle) {
        finishModernCycle(pipeline, input.tickReport, input.services, input.sinks, input.finalizer, false, pipeline.emergencyActive, pipeline.stopReason);
        return pipeline;
    }

    pipeline = RuntimeBodyPoseExtractionStage::execute(std::move(pipeline), input.services);
    if (!pipeline.continueCycle) {
        finishModernCycle(pipeline, input.tickReport, input.services, input.sinks, input.finalizer, false, pipeline.emergencyActive, pipeline.stopReason);
        return pipeline;
    }

    pipeline = RuntimeBodySchemaStage::execute(std::move(pipeline), pipeline.perceivedBodyPose, pipeline.avatarBodyPose);
    if (!pipeline.continueCycle) {
        finishModernCycle(pipeline, input.tickReport, input.services, input.sinks, input.finalizer, false, pipeline.emergencyActive, pipeline.stopReason);
        return pipeline;
    }

    pipeline = RuntimeSafetyStage::execute(std::move(pipeline), input.services, input.context);
    if (!pipeline.continueCycle) {
        finishModernCycle(pipeline, input.tickReport, input.services, input.sinks, input.finalizer, false, pipeline.emergencyActive, pipeline.stopReason);
        return pipeline;
    }

    const auto tRender = std::chrono::steady_clock::now();
    pipeline = RuntimeRenderStage::execute(std::move(pipeline), input.services, input.tickReport);
    const bool renderOk = input.finalizer.scheduler.endTask(RuntimeRenderStage::name(), tRender, input.tickReport);
    if (renderOk && pipeline.continueCycle) {
        pipeline.renderStatus = RuntimeRenderStage::validate(true, "render_stage_unhealthy");
    } else if (!renderOk && pipeline.continueCycle) {
        pipeline.renderStatus = RuntimeStageStatus::degraded("render_budget_exceeded");
        if (input.sinks.recordDroppedFrame) input.sinks.recordDroppedFrame();
        if (input.sinks.logWarn) input.sinks.logWarn("⚠️ Render budget exceeded — jitter risk. task=render");
    }
    if (!pipeline.continueCycle) {
        finishModernCycle(pipeline, input.tickReport, input.services, input.sinks, input.finalizer, false, pipeline.emergencyActive, pipeline.stopReason);
        return pipeline;
    }

    pipeline = RuntimeModalityPhaseLockStage::execute(std::move(pipeline));
    if (!pipeline.continueCycle) {
        finishModernCycle(pipeline, input.tickReport, input.services, input.sinks, input.finalizer, false, pipeline.emergencyActive, pipeline.stopReason);
        return pipeline;
    }

    pipeline.stimulusStatus = RuntimeStimulusStage::validate(true, "stimulus_stage_unavailable");
    if (!pipeline.stimulusStatus.continueCycle) {
        finishModernCycle(pipeline, input.tickReport, input.services, input.sinks, input.finalizer, false, pipeline.stimulusStatus.emergencyActive, pipeline.stimulusStatus.reason);
        return pipeline;
    }

    pipeline = RuntimeStimulusStage::execute(std::move(pipeline), input.services);
    if (!pipeline.continueCycle) {
        if (pipeline.stopReason == "unsafe_stimulus_rejected") {
            if (input.sinks.auditRejectedStimulus) input.sinks.auditRejectedStimulus(toLegacyCommand(pipeline.stimulusCommand));
        }
        finishModernCycle(pipeline, input.tickReport, input.services, input.sinks, input.finalizer, false, pipeline.emergencyActive, pipeline.stopReason);
        return pipeline;
    }

    pipeline.feedbackChipFrame = pipeline.chipFrame;
    pipeline.chipFrame = pipeline.feedbackChipFrame;
    pipeline = RuntimeClosedLoopFeedbackStage::execute(std::move(pipeline));
    const bool feedbackMissing = !pipeline.closedLoopFeedback.safetyVerified;
    if (input.services.neuralChipHal.requiresClosedLoopFeedback() && feedbackMissing) {
        finishModernCycle(pipeline, input.tickReport, input.services, input.sinks, input.finalizer, false, true, "mandatory_closed_loop_feedback_absent");
        return pipeline;
    }

    pipeline = RuntimePerceptionCorrectionStage::execute(std::move(pipeline));
    if (!pipeline.continueCycle) {
        finishModernCycle(pipeline, input.tickReport, input.services, input.sinks, input.finalizer, false, pipeline.emergencyActive, pipeline.stopReason);
        return pipeline;
    }

    pipeline = RuntimePostCorrectionSafetyStage::execute(std::move(pipeline));
    if (!pipeline.continueCycle) {
        finishModernCycle(pipeline, input.tickReport, input.services, input.sinks, input.finalizer, false, pipeline.emergencyActive, pipeline.stopReason);
        return pipeline;
    }

    RuntimeCycleFinalizer::finish(
        input.tickReport,
        pipeline,
        input.services,
        input.sinks,
        input.finalizer,
        RuntimeCycleFinalizer::Options{true, false, "nominal_runtime", true});
    pipeline.continueCycle = true;
    pipeline.emergencyActive = false;
    pipeline.stopReason = "nominal_runtime";
    return pipeline;
}


} // namespace dnxr
