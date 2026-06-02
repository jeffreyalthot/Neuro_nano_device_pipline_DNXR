#include "RuntimePipelineRunner.h"

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

void RuntimePipelineRunner::finishCycle(RuntimePipelineFrame& frame,
                                        SchedulerTickReport& tickReport,
                                        const RuntimeCycleFinalizer::Input& input,
                                        bool safeDecision,
                                        bool emergencyActive,
                                        const std::string& reason,
                                        bool emitBudgetWarning) {
    RuntimeCycleFinalizer::finish(
        tickReport,
        safeDecision,
        emergencyActive,
        reason,
        input,
        emitBudgetWarning);
    frame.continueCycle = false;
    frame.emergencyActive = emergencyActive;
    frame.stopReason = reason;
}

RuntimePipelineFrame RuntimePipelineRunner::run(const RunInput& input) const {
    RuntimePipelineFrame pipeline;

    const auto tAcq = std::chrono::steady_clock::now();
    pipeline = input.services
        ? RuntimeAcquisitionStage::execute(input.context, *input.services)
        : RuntimeAcquisitionStage::execute(input.context, input.stages.acquisition);
    input.stages.finalizer.scheduler.endTask(RuntimeAcquisitionStage::name(), tAcq, input.tickReport);
    if (!pipeline.continueCycle) {
        if (pipeline.emergencyActive) {
            if (input.triggerEmergency) input.triggerEmergency();
            if (input.logWarn) input.logWarn("⚠️  Physiological guard triggered.");
            finishCycle(pipeline, input.tickReport, input.stages.finalizer, false, true, pipeline.stopReason);
            if (input.throttleEmergency) input.throttleEmergency();
            return pipeline;
        }
        if (input.logWarn) input.logWarn("⚠️  Empty neural signal frame.");
        finishCycle(pipeline, input.tickReport, input.stages.finalizer, false, pipeline.acquisitionStatus.emergencyActive, pipeline.stopReason);
        return pipeline;
    }

    const auto tSignal = std::chrono::steady_clock::now();
    pipeline = input.services
        ? RuntimeSignalStage::execute(std::move(pipeline), *input.services)
        : RuntimeSignalStage::execute(std::move(pipeline), input.stages.signal);
    input.stages.finalizer.scheduler.endTask(RuntimeSignalStage::name(), tSignal, input.tickReport);
    if (!pipeline.continueCycle) {
        finishCycle(pipeline, input.tickReport, input.stages.finalizer, false, pipeline.signalStatus.emergencyActive, pipeline.stopReason);
        return pipeline;
    }

    pipeline.normalizedTelemetryRisk = normalizedTelemetryRiskVector(pipeline.telemetry);

    const auto tInference = std::chrono::steady_clock::now();
    pipeline = input.services
        ? RuntimeInferenceStage::execute(std::move(pipeline), *input.services)
        : RuntimeInferenceStage::execute(std::move(pipeline), input.stages.inference);
    const bool inferenceOk = input.stages.finalizer.scheduler.endTask(RuntimeInferenceStage::name(), tInference, input.tickReport);
    if (!inferenceOk) {
        if (input.recordDroppedFrame) input.recordDroppedFrame();
        if (input.logWarn) input.logWarn("⚠️ Inference budget exceeded — frame degraded, skipping render. task=inference");
        finishCycle(pipeline, input.tickReport, input.stages.finalizer, true, false, "inference_budget_exceeded");
        return pipeline;
    }

    if (input.publishPipelineState) input.publishPipelineState(pipeline);

    pipeline = input.services
        ? RuntimeCalibrationStage::execute(std::move(pipeline), *input.services)
        : RuntimeCalibrationStage::execute(std::move(pipeline), input.stages.calibration);
    if (!pipeline.continueCycle) {
        finishCycle(pipeline, input.tickReport, input.stages.finalizer, false, pipeline.calibrationStatus.emergencyActive, pipeline.stopReason);
        return pipeline;
    }

    pipeline = input.services
        ? RuntimeImmersionFrameStage::execute(std::move(pipeline), *input.services)
        : RuntimeImmersionFrameStage::execute(std::move(pipeline), input.stages.immersionFrame);
    if (!pipeline.continueCycle) {
        finishCycle(pipeline, input.tickReport, input.stages.finalizer, false, pipeline.immersionFrameStatus.emergencyActive, pipeline.stopReason);
        return pipeline;
    }

    pipeline = RuntimeSafetyStage::execute(std::move(pipeline), input.stages.safety);
    if (!pipeline.continueCycle) {
        finishCycle(pipeline, input.tickReport, input.stages.finalizer, false, pipeline.emergencyActive, pipeline.stopReason);
        return pipeline;
    }

    if (input.recordReplayFrame) input.recordReplayFrame(pipeline.cleanSignals, input.context.timestamp);

    pipeline = RuntimeFullDiveStage::execute(std::move(pipeline), input.stages.fullDive);
    if (!pipeline.continueCycle) {
        finishCycle(pipeline, input.tickReport, input.stages.finalizer, false, pipeline.emergencyActive, pipeline.stopReason);
        return pipeline;
    }

    pipeline = RuntimeRenderStage::execute(std::move(pipeline), input.makeRenderStageInput(input.tickReport));
    if (!pipeline.continueCycle) {
        finishCycle(pipeline, input.tickReport, input.stages.finalizer, false, pipeline.emergencyActive, pipeline.stopReason);
        return pipeline;
    }

    pipeline.stimulusStatus = RuntimeStimulusStage::validate(
        input.services ||
            (static_cast<bool>(input.stages.stimulus.buildCommand) &&
             static_cast<bool>(input.stages.stimulus.writeSafeStimulus)),
        "stimulus_stage_unavailable");
    if (!pipeline.stimulusStatus.continueCycle) {
        finishCycle(pipeline, input.tickReport, input.stages.finalizer, false, pipeline.stimulusStatus.emergencyActive, pipeline.stimulusStatus.reason);
        return pipeline;
    }

    pipeline = input.services
        ? RuntimeStimulusStage::execute(std::move(pipeline), *input.services)
        : RuntimeStimulusStage::execute(std::move(pipeline), input.stages.stimulus);
    if (!pipeline.continueCycle) {
        if (pipeline.stopReason == "unsafe_stimulus_rejected") {
            if (input.auditRejectedStimulus) input.auditRejectedStimulus(pipeline.stimulusCommand);
        }
        finishCycle(pipeline, input.tickReport, input.stages.finalizer, false, pipeline.emergencyActive, pipeline.stopReason);
        return pipeline;
    }

    RuntimeCycleFinalizer::finish(input.tickReport, true, false, "nominal_runtime", input.stages.finalizer, true);
    pipeline.continueCycle = true;
    pipeline.emergencyActive = false;
    pipeline.stopReason = "nominal_runtime";
    return pipeline;
}

} // namespace dnxr
