#include "RuntimeImmersionFrameStage.h"

#include "RuntimeStageServices.h"
#include "../immersion/MultisensoryClock.h"
#include "../neurotech/DeepImmersionPlatform.h"

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

RuntimePipelineFrame RuntimeImmersionFrameStage::execute(RuntimePipelineFrame frame, RuntimeStageServices& services) {
    if (!frame.continueCycle) {
        return frame;
    }

    frame.bodyCommand = services.deepImmersion.runFullBodyControlTick(frame.cleanSignals, frame.cognitive);

    const auto& world = frame.canonicalWorldFrame;
    std::vector<float> sensoryFrame{
        world.scene.luminance01,
        world.scene.contrast01,
        world.scene.nearObstacle01,
        world.scene.horizonStability01,
        world.audio.loudness01,
        world.audio.leftRight01,
        world.audio.frontBack01,
        world.contacts.pressure01,
        world.contacts.texture01,
        world.pose.ownership01,
        world.pose.handConfidence01,
        world.motion.angularVelocity01,
        world.motion.balanceError01
    };
    if (!sensoryFrame.empty() && !frame.calibratedFeatures.empty()) {
        sensoryFrame[0] = (sensoryFrame[0] * 0.85f) + (frame.calibratedFeatures.front() * 0.15f);
    }

    frame.sensorySync = services.multisensoryClock.alignVector(sensoryFrame, frame.cycleId, frame.timestamp * 1000.0, 2.0);
    frame.sensoryFrame = frame.sensorySync.alignedFrame;
    frame.immersionFrameStatus = validate(true, "immersion_frame_stage_unhealthy");
    return frame;
}

} // namespace dnxr
