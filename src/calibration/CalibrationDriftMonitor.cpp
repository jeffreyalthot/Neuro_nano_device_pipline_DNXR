#include "CalibrationDriftMonitor.h"

#include <algorithm>
#include <cmath>
#include <numeric>

namespace dnxr {
namespace {

float finiteOrDefault(float value, float fallback) {
    return std::isfinite(value) ? value : fallback;
}

double finiteOrDefault(double value, double fallback) {
    return std::isfinite(value) ? value : fallback;
}

} // namespace

void CalibrationDriftMonitor::setBaseline(const UserBaselineProfile& profile) {
    baseline_ = profile;
    baseline_.signalQuality = clamp01(finiteOrDefault(profile.signalQuality, 0.0f));
    baseline_.meanActivation = finiteOrDefault(profile.meanActivation, 0.0f);
    baseline_.variance = std::max(0.0f, finiteOrDefault(profile.variance, 0.0f));
    baseline_.calibratedAtSeconds = finiteOrDefault(profile.calibratedAtSeconds, 0.0);
    previousDriftScore_ = 0.0f;
    previousTimestampSeconds_ = baseline_.calibratedAtSeconds;
    hasPreviousAssessment_ = false;
}

UserBaselineProfile CalibrationDriftMonitor::baseline() const {
    return baseline_;
}

UserBaselineProfile CalibrationDriftMonitor::buildBaseline(
    const std::vector<float>& calibratedSignal,
    float signalQuality,
    double timestampSeconds,
    const std::string& profileId) const {

    UserBaselineProfile profile;
    profile.profileId = profileId.empty() ? "baseline" : profileId;
    profile.meanActivation = meanOf(calibratedSignal);
    profile.variance = varianceOf(calibratedSignal, profile.meanActivation);
    profile.signalQuality = clamp01(finiteOrDefault(signalQuality, 0.0f));
    profile.calibratedAtSeconds = finiteOrDefault(timestampSeconds, 0.0);
    profile.sampleCount = calibratedSignal.size();
    return profile;
}

CalibrationDriftAssessment CalibrationDriftMonitor::assess(
    const std::vector<float>& calibratedSignal,
    float signalQuality,
    float intentConfidence,
    float fatigue,
    double timestampSeconds) {

    CalibrationDriftAssessment assessment;
    const double timestamp = finiteOrDefault(timestampSeconds, baseline_.calibratedAtSeconds);
    assessment.calibrationAgeSeconds = std::max(0.0, timestamp - baseline_.calibratedAtSeconds);
    assessment.fatigueIndex = clamp01(finiteOrDefault(fatigue, 0.0f));

    if (calibratedSignal.empty() || baseline_.sampleCount == 0U) {
        assessment.driftScore = 1.0f;
        assessment.confidenceEntropy = 1.0f;
        assessment.requiresRecalibration = true;
        assessment.blockCriticalActions = true;
        assessment.reason = "calibration_missing_signal_or_baseline";
        return assessment;
    }

    const float currentMean = meanOf(calibratedSignal);
    const float currentVariance = varianceOf(calibratedSignal, currentMean);
    const float meanDrift = clamp01(std::abs(currentMean - baseline_.meanActivation));
    const float varianceDrift = clamp01(std::abs(currentVariance - baseline_.variance));
    const float qualityDrop = clamp01(baseline_.signalQuality - clamp01(finiteOrDefault(signalQuality, 0.0f)));
    assessment.confidenceEntropy = clamp01(1.0f - clamp01(finiteOrDefault(intentConfidence, 0.0f)));

    assessment.driftScore = clamp01(
        (meanDrift * 0.30f) +
        (varianceDrift * 0.25f) +
        (qualityDrop * 0.20f) +
        (assessment.fatigueIndex * 0.15f) +
        (assessment.confidenceEntropy * 0.10f));

    if (hasPreviousAssessment_) {
        const double deltaSeconds = std::max(0.001, timestamp - previousTimestampSeconds_);
        assessment.driftVelocity = clamp01(std::abs(assessment.driftScore - previousDriftScore_) /
                                           static_cast<float>(deltaSeconds));
    }

    hasPreviousAssessment_ = true;
    previousDriftScore_ = assessment.driftScore;
    previousTimestampSeconds_ = timestamp;

    if (assessment.driftScore >= 0.75f || assessment.driftVelocity >= 0.50f) {
        assessment.requiresRecalibration = true;
        assessment.blockCriticalActions = true;
        assessment.reason = assessment.driftVelocity >= 0.50f
            ? "calibration_drift_velocity_high"
            : "calibration_drift_high";
    } else if (assessment.driftScore >= 0.45f || assessment.calibrationAgeSeconds > 1800.0) {
        assessment.requiresRecalibration = true;
        assessment.reason = assessment.calibrationAgeSeconds > 1800.0
            ? "calibration_age_recalibration_due"
            : "calibration_drift_recalibration_due";
    } else {
        assessment.reason = "calibration_stable";
    }

    return assessment;
}

float CalibrationDriftMonitor::meanOf(const std::vector<float>& values) {
    if (values.empty()) {
        return 0.0f;
    }
    return std::accumulate(values.begin(), values.end(), 0.0f) / static_cast<float>(values.size());
}

float CalibrationDriftMonitor::varianceOf(const std::vector<float>& values, float mean) {
    if (values.empty()) {
        return 0.0f;
    }
    float total = 0.0f;
    for (float value : values) {
        const float delta = value - mean;
        total += delta * delta;
    }
    return total / static_cast<float>(values.size());
}

float CalibrationDriftMonitor::clamp01(float value) {
    return std::max(0.0f, std::min(1.0f, finiteOrDefault(value, 0.0f)));
}

} // namespace dnxr
