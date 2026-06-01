#pragma once

#include <cstddef>
#include <string>
#include <vector>

namespace dnxr {

struct UserBaselineProfile {
    std::string profileId = "default_baseline";
    float meanActivation = 0.0f;
    float variance = 0.0f;
    float signalQuality = 1.0f;
    double calibratedAtSeconds = 0.0;
    std::size_t sampleCount = 0;
};

struct CalibrationDriftAssessment {
    float driftScore = 0.0f;
    float driftVelocity = 0.0f;
    float fatigueIndex = 0.0f;
    float confidenceEntropy = 0.0f;
    double calibrationAgeSeconds = 0.0;
    bool requiresRecalibration = false;
    bool blockCriticalActions = false;
    std::string reason = "calibration_stable";
};

class CalibrationDriftMonitor {
public:
    void setBaseline(const UserBaselineProfile& profile);
    UserBaselineProfile baseline() const;

    UserBaselineProfile buildBaseline(
        const std::vector<float>& calibratedSignal,
        float signalQuality,
        double timestampSeconds,
        const std::string& profileId) const;

    CalibrationDriftAssessment assess(
        const std::vector<float>& calibratedSignal,
        float signalQuality,
        float intentConfidence,
        float fatigue,
        double timestampSeconds);

private:
    UserBaselineProfile baseline_;
    float previousDriftScore_ = 0.0f;
    double previousTimestampSeconds_ = 0.0;
    bool hasPreviousAssessment_ = false;

    static float meanOf(const std::vector<float>& values);
    static float varianceOf(const std::vector<float>& values, float mean);
    static float clamp01(float value);
};

} // namespace dnxr
