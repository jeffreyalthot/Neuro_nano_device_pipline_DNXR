#pragma once

#include "RuntimeStageTypes.h"
#include "RuntimePipelineDiagnostics.h"
#include "RuntimePipelineImmersionTypes.h"
#include "RuntimePipelineNeuralTypes.h"
#include "RuntimePipelineSafetyTypes.h"
#include "../full_dive/FullDiveNeuralFrame.h"
#include "../full_dive/FiveSenseCoherenceEngine.h"
#include "../full_dive/BodySchemaEngine.h"
#include "../hardware/ClosedLoopNeuralFeedbackReader.h"
#include "../full_dive/ModalityPhaseLockEngine.h"
#include "../full_dive/PerceptionDivergenceDetector.h"

#include <cstdint>
#include <string>
#include <vector>

namespace dnxr {

struct RuntimePipelineFrame {
    std::uint64_t cycleId = 0;
    double timestamp = 0.0;
    double deltaTimeSeconds = 0.0;

    std::vector<float> rawNeuralSignals;
    std::vector<float> cleanSignals;
    std::vector<float> features;
    std::vector<float> calibratedFeatures;
    std::vector<float> sensoryFrame;
    std::vector<float> telemetry;
    std::vector<float> normalizedTelemetryRisk;
    NeuralChipFrame chipFrame;

    std::string symbolicState;
    BiometricFrame biometric;
    CognitiveState cognitive;
    XRAdaptationFrame adaptation;
    NeuroPipelineDiagnostics diagnostics;
    CalibrationDriftAssessment calibrationDrift;
    MultisensoryAlignmentReport sensorySync;
    XRFullBodyCommand bodyCommand;
    NeuralSensoryFrame neuralSensoryFrame;
    SafetyDecision safetyDecision;
    SafetyDecision fusedSafetyDecision;
    RealityAnchorPlan realityAnchorPlan;
    FullDiveRuntimeSnapshot fullDiveSnapshot;
    SafeNeuralStimulusCommand stimulusCommand;
    FullDiveNeuralFrame fullDiveNeuralFrame;
    FiveSenseCoherenceReport fiveSenseCoherenceReport;
    BodySchemaReport bodySchemaReport;
    BodyPoseState perceivedBodyPose;
    BodyPoseState avatarBodyPose;
    bool bodyPoseExtractionComplete = false;
    ClosedLoopFeedback closedLoopFeedback;
    PhaseLockReport phaseLockReport;
    PerceptionDivergenceReport perceptionDivergenceReport;

    RuntimeStageStatus acquisitionStatus = RuntimeStageStatus::ok("acquisition");
    RuntimeStageStatus signalStatus = RuntimeStageStatus::ok("signal");
    RuntimeStageStatus inferenceStatus = RuntimeStageStatus::ok("inference");
    RuntimeStageStatus calibrationStatus = RuntimeStageStatus::ok("calibration");
    RuntimeStageStatus immersionFrameStatus = RuntimeStageStatus::ok("immersion_frame");
    RuntimeStageStatus safetyStatus = RuntimeStageStatus::ok("safety");
    RuntimeStageStatus fullDiveStatus = RuntimeStageStatus::ok("fulldive");
    RuntimeStageStatus renderStatus = RuntimeStageStatus::ok("render");
    RuntimeStageStatus stimulusStatus = RuntimeStageStatus::ok("stimulus");

    bool emergencyActive = false;
    bool continueCycle = true;
    std::string stopReason;
};

} // namespace dnxr
