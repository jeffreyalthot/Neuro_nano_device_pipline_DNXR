#include "RuntimeImmersionFrameStage.h"

namespace dnxr {

const char* RuntimeImmersionFrameStage::name() {
    return "immersion_frame";
}

RuntimeStageStatus RuntimeImmersionFrameStage::validate(bool healthy, const std::string& failureReason) {
    return healthy ? RuntimeStageStatus::ok(name()) : RuntimeStageStatus::degraded(failureReason);
}

RuntimePipelineFrame RuntimeImmersionFrameStage::execute(RuntimePipelineFrame frame, const Input& input) {
    if (!frame.continueCycle) {
        return frame;
    }

    frame.immersionFrameStatus = validate(
        input.runFullBodyControlTick && input.buildImmersiveFrame && input.alignVector,
        "immersion_frame_stage_unavailable");
    if (!frame.immersionFrameStatus.continueCycle) {
        frame.continueCycle = false;
        frame.stopReason = frame.immersionFrameStatus.reason;
        return frame;
    }

    frame.bodyCommand = input.runFullBodyControlTick(frame.cleanSignals, frame.cognitive);
    frame.neuralSensoryFrame = input.buildImmersiveFrame(
        frame.cognitive,
        frame.bodyCommand,
        frame.adaptation,
        frame.diagnostics);

    std::vector<float> sensoryFrame = frame.neuralSensoryFrame.toVector();
    if (!sensoryFrame.empty() && !frame.calibratedFeatures.empty()) {
        sensoryFrame[0] = (sensoryFrame[0] * 0.85f) + (frame.calibratedFeatures.front() * 0.15f);
    }

    frame.sensorySync = input.alignVector(sensoryFrame, frame.cycleId, input.timestampMs, input.phaseToleranceMs);
    frame.sensoryFrame = frame.sensorySync.alignedFrame;

    if (frame.sensorySync.degradationApplied) {
        if (input.recordTrace) {
            input.recordTrace(
                frame.cycleId,
                "MultisensoryClock",
                "alignment_degraded",
                frame.sensorySync.maxObservedPhaseErrorMs,
                1.0f - frame.sensorySync.recommendedIntensityScale,
                true,
                frame.sensorySync.reason);
        }
        if (input.logWarn) {
            input.logWarn("[MultisensoryClock] " + frame.sensorySync.reason +
                          " phase_ms=" + std::to_string(frame.sensorySync.maxObservedPhaseErrorMs) +
                          " confidence=" + std::to_string(frame.sensorySync.confidence) +
                          " scale=" + std::to_string(frame.sensorySync.recommendedIntensityScale));
        }
    }

    frame.immersionFrameStatus = validate(true, "immersion_frame_stage_unhealthy");
    return frame;
}

} // namespace dnxr
