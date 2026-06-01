#pragma once

#include "../calibration/CalibrationDriftMonitor.h"
#include "../immersion/MultisensoryClock.h"
#include "../neurotech/DeepImmersionPlatform.h"
#include "../safety/RealityAnchorSystem.h"
#include "../safety/SafetyGovernor.h"
#include "../utils/RuntimeMetrics.h"

namespace dnxr {

struct FullDiveRuntimeInput {
    CognitiveState cognitive;
    BiometricFrame biometric;
    NeuroPipelineDiagnostics diagnostics;
    CalibrationDriftAssessment calibrationDrift;
    SafetyDecision safetyDecision;
    RealityAnchorPlan realityAnchorPlan;
    MultisensoryAlignmentReport sensorySync;
    RuntimeMetricsSnapshot runtimeMetrics;
    double deltaTimeSeconds = 0.0;
};

} // namespace dnxr
