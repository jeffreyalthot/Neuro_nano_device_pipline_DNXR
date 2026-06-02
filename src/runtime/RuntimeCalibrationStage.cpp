#include "RuntimeCalibrationStage.h"

namespace dnxr {

const char* RuntimeCalibrationStage::name() {
    return "calibration";
}

RuntimeStageStatus RuntimeCalibrationStage::validate(bool healthy, const std::string& failureReason) {
    return healthy ? RuntimeStageStatus::ok(name()) : RuntimeStageStatus::degraded(failureReason);
}

RuntimePipelineFrame RuntimeCalibrationStage::execute(RuntimePipelineFrame frame, const Input& input) {
    if (!frame.continueCycle) {
        return frame;
    }

    frame.calibrationStatus = validate(input.calibrateForImmersion && input.assessDrift, "calibration_stage_unavailable");
    if (!frame.calibrationStatus.continueCycle) {
        frame.continueCycle = false;
        frame.stopReason = frame.calibrationStatus.reason;
        return frame;
    }

    frame.calibratedFeatures = input.calibrateForImmersion(frame.features, input.userSensitivity, frame.timestamp);

    if (input.initializeBaseline && input.buildBaseline && input.setBaseline) {
        input.setBaseline(input.buildBaseline(
            frame.calibratedFeatures,
            frame.diagnostics.neuralSignalQuality,
            frame.timestamp,
            "runtime_baseline"));
    }

    frame.calibrationDrift = input.assessDrift(
        frame.calibratedFeatures,
        frame.diagnostics.neuralSignalQuality,
        frame.diagnostics.intentConfidence,
        frame.cognitive.fatigue,
        frame.timestamp);

    if (frame.calibrationDrift.requiresRecalibration) {
        if (input.logWarn) {
            input.logWarn("[CalibrationDriftMonitor] " + frame.calibrationDrift.reason +
                          " drift=" + std::to_string(frame.calibrationDrift.driftScore) +
                          " velocity=" + std::to_string(frame.calibrationDrift.driftVelocity) +
                          " age_s=" + std::to_string(frame.calibrationDrift.calibrationAgeSeconds));
        }
        if (input.recordTrace) {
            input.recordTrace(
                frame.cycleId,
                "CalibrationDriftMonitor",
                "recalibration_recommended",
                frame.calibrationDrift.calibrationAgeSeconds,
                frame.calibrationDrift.driftScore,
                frame.calibrationDrift.blockCriticalActions,
                frame.calibrationDrift.reason);
        }
        if (frame.calibrationDrift.blockCriticalActions && input.resetStabilization) {
            input.resetStabilization();
        }
    }

    frame.calibrationStatus = validate(true, "calibration_stage_unhealthy");
    return frame;
}

} // namespace dnxr
