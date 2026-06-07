#include "RuntimePostCorrectionSafetyStage.h"

#include <algorithm>

namespace dnxr {

const char* RuntimePostCorrectionSafetyStage::name() { return "post_correction_safety"; }

RuntimePipelineFrame RuntimePostCorrectionSafetyStage::execute(RuntimePipelineFrame frame,
                                                              const ExpectedPerceptionModel& expectedModel,
                                                              const PerceptionDivergenceDetector& divergenceDetector,
                                                              const FullDiveSafetySupervisor& safetySupervisor) {
    const auto expected = expectedModel.predictFixed(frame.fullDiveNeuralFrame);
    frame.perceptionDivergenceReport = divergenceDetector.detectFixed(expected, frame.closedLoopFeedback,
                                                                      frame.telemetry.size() > 1 ? frame.telemetry[1] : 0.0f,
                                                                      frame.chipFrame.acquisitionLatencyMs);
    if (frame.perceptionDivergenceReport.mustStop) {
        frame.continueCycle = false;
        frame.emergencyActive = true;
        frame.stopReason = "post_correction_divergence_requires_decompression";
        frame.safetyStatus = RuntimeStageStatus::emergency(frame.stopReason);
        return frame;
    }
    const auto decision = safetySupervisor.evaluate(frame.fullDiveNeuralFrame);
    if (!decision.allowed) {
        frame.continueCycle = false;
        frame.emergencyActive = decision.requiresEmergencyStop;
        frame.stopReason = decision.reason;
        frame.safetyStatus = RuntimeStageStatus::emergency(frame.stopReason);
        return frame;
    }
    return frame;
}

} // namespace dnxr
