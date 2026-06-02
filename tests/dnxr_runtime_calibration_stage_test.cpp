#include "../src/runtime/RuntimeCalibrationStage.h"

#include <cassert>

int main() {
    dnxr::RuntimePipelineFrame frame;
    frame.cycleId = 7;
    frame.timestamp = 10.0;
    frame.features = {0.2f, 0.4f};
    frame.diagnostics.neuralSignalQuality = 0.9f;
    frame.diagnostics.intentConfidence = 0.8f;
    frame.cognitive.fatigue = 0.1f;

    bool baselineSet = false;
    bool traceRecorded = false;
    bool reset = false;
    auto out = dnxr::RuntimeCalibrationStage::execute(frame, dnxr::RuntimeCalibrationStage::Input{
        [](const std::vector<float>& features, float sensitivity, double) {
            auto calibrated = features;
            for (float& value : calibrated) value *= sensitivity;
            return calibrated;
        },
        [](const std::vector<float>&, float quality, double ts, const std::string& label) {
            dnxr::UserBaselineProfile baseline;
            baseline.signalQuality = quality;
            baseline.calibratedAtSeconds = ts;
            baseline.profileId = label;
            return baseline;
        },
        [&baselineSet](const dnxr::UserBaselineProfile&) { baselineSet = true; },
        [](const std::vector<float>&, float, float, float, double) {
            dnxr::CalibrationDriftAssessment drift;
            drift.requiresRecalibration = true;
            drift.blockCriticalActions = true;
            drift.driftScore = 0.7f;
            drift.reason = "test_drift";
            return drift;
        },
        [&reset]() { reset = true; },
        [&traceRecorded](std::uint64_t, const std::string&, const std::string&, double, float, bool, const std::string&) {
            traceRecorded = true;
        },
        {},
        2.0f,
        true
    });

    assert(out.continueCycle);
    assert(out.calibratedFeatures.size() == 2);
    assert(out.calibratedFeatures[0] == 0.4f);
    assert(out.calibrationDrift.requiresRecalibration);
    assert(baselineSet);
    assert(traceRecorded);
    assert(reset);

    auto unavailable = dnxr::RuntimeCalibrationStage::execute(frame, dnxr::RuntimeCalibrationStage::Input{});
    assert(!unavailable.continueCycle);
    assert(unavailable.stopReason == "calibration_stage_unavailable");
    return 0;
}
