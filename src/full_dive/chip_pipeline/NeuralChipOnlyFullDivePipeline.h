#pragma once

#include "full_dive/auditory_chip/AuditoryTonotopicEncoder.h"
#include "full_dive/chip_pipeline/ClosedLoopPerceptionErrorController.h"
#include "full_dive/gustatory_chip/NeuralGustatoryEncoder.h"
#include "full_dive/motor_chip/AvatarKinematicRigController.h"
#include "full_dive/motor_chip/MotorCortexReadFrameAdapter.h"
#include "full_dive/motor_chip/MotorIntentConfidenceGate.h"
#include "full_dive/motor_chip/AvatarSafetyPoseController.h"
#include "full_dive/olfactory_chip/NeuralOlfactoryEncoder.h"
#include "full_dive/safety/ChipRealityAnchorHardChannel.h"
#include "full_dive/safety/ExperimentalChemicalChannelGate.h"
#include "full_dive/safety/ModalityExposureLedger.h"
#include "full_dive/safety/PerModalityIsolationBroker.h"
#include "full_dive/safety/PerModalityRateAndDosePolicy.h"
#include "full_dive/safety/PerModalitySafetyEnvelope.h"
#include "full_dive/sync/MultisensoryPhaseLockOrchestrator.h"
#include "full_dive/touch_chip/NeuralBodySenseEncoders.h"
#include "full_dive/touch_chip/NeuralVirtualEpidermisEncoder.h"
#include "full_dive/visual_chip/VisualCorticalWorldEncoder.h"
#include "neural_chip/ChipCapabilityContract.h"
#include "neural_chip/FrameLifecycleGuard.h"
#include "neural_chip/FullDiveChipFrameDiff.h"
#include "neural_chip/NeuralChipBus.h"
#include "neural_chip/NeuralChipAckHandshake.h"
#include "neural_chip/NeuralChipEmergencyLatch.h"
#include "neural_chip/NeuralChipFrameCommitter.h"
#include "neural_chip/NeuralChipPipelineDecisionTrace.h"
#include "neural_chip/PipelineResumeController.h"
#include "neural_chip/NeuralChipSessionState.h"
#include "neural_chip/NeuralChipSessionSupervisor.h"
#include "neural_chip/NeuralChipReplayRecorder.h"
#include "neural_chip/NeuralVectorSlewRateLimiter.h"
#include "neural_chip/NeuralVectorValidator.h"
#include "neural_chip/PatientModalityPolicyGate.h"
#include "neural_chip/PatientSpecificNeuralMap.h"
#include "neural_chip/SafeChipFrameHoldBuffer.h"

namespace dnxr {

struct NeuralChipOnlyPipelineConfig {
    ChipCapabilities capabilities{};
    PatientSpecificNeuralMap patientMap = PatientSpecificNeuralMap::simulationDefaults();
    bool enableExperimentalChemicalChannels = false;
    bool enableResearchVision = false;
    bool enableResearchAuditory = false;
    bool enableResearchVestibular = false;
    bool simulationOnly = true;
    bool allowStimulusDispatch = false;
    bool experimentalAcknowledgement = false;
    bool researchModeAcknowledgement = false;
    bool strictConfig = false;

    static NeuralChipOnlyPipelineConfig simulationSafeDefaults();
};

struct NeuralChipOnlyWorldFrame {
    std::uint64_t frameId = 0;
    double timestampMs = 0.0;
    VirtualSceneFrame scene;
    SpatialAudioFrame audio;
    AvatarContactFrame contacts;
    AvatarPoseFrame pose;
    AvatarMotionFrame motion;
    VirtualOdorField odor;
    VirtualTasteFrame taste;
    PatientVisualMap visualMap;
    PatientAuditoryMap auditoryMap;
    PatientSomatosensoryMap touchMap;
    PatientOlfactoryMap olfactoryMap;
    PatientGustatoryMap gustatoryMap;
    VisualSafetyState visualSafety;
    AuditorySafetyState auditorySafety;
    TouchSafetyState touchSafety;
    OlfactorySafetyState olfactorySafety;
    GustatorySafetyState gustatorySafety;
    NeuralRealityAnchorVector realityAnchor;
    PatientSpecificNeuralMap patientMap = PatientSpecificNeuralMap::simulationDefaults();
    NeuralChipLatencyBudget latencyBudget;
};

class NeuralChipOnlyFullDivePipeline {
public:
    explicit NeuralChipOnlyFullDivePipeline(NeuralChipBus& chipBus);
    NeuralChipOnlyFullDivePipeline(NeuralChipBus& chipBus, const NeuralChipOnlyPipelineConfig& config);

    bool step(const NeuralChipOnlyWorldFrame& world);
    const AvatarRigCommandFrame& lastRigCommand() const;
    const NeuralChipSessionState& sessionState() const;
    const NeuralChipPipelineDecisionTrace& decisionTrace() const;
    const NeuralChipReplayRecorder& replayRecorder() const;
    NeuralChipEmergencyLatch& emergencyLatch();
    void injectSimulatedAck(std::uint64_t frameId, float quality01);

private:
    void recordDiff(const char* stage, const FullDiveChipFrame& before, const FullDiveChipFrame& after);
    bool rejectWithHold(FullDiveChipFrame frame, const char* reason, float ramp01, const ChipReadFrame& read = {});
    void recordReplay(const ChipReadFrame& read, const FullDiveChipFrame& candidate, const FrameCommitResult& result, const char* holdReason);
    bool criticalSafetyViolation(const FullDiveChipFrame& frame, const char** reason) const;
    void markAccepted(const FullDiveChipFrame& frame);
    PatientSpecificNeuralMap effectivePatientMap(const NeuralChipOnlyWorldFrame& world) const;

    NeuralChipBus& chipBus_;
    VisualCorticalWorldEncoder visualEncoder_{};
    AuditoryTonotopicEncoder auditoryEncoder_{};
    NeuralVirtualEpidermisEncoder epidermisEncoder_{};
    NeuralProprioceptionEncoder proprioceptionEncoder_{};
    NeuralVestibularEncoder vestibularEncoder_{};
    NeuralOlfactoryEncoder olfactoryEncoder_{};
    NeuralGustatoryEncoder gustatoryEncoder_{};
    MotorIntentDecoder motorDecoder_{};
    MotorIntentConfidenceGate motorConfidenceGate_{};
    AvatarKinematicRigController avatarRig_{};
    AvatarSafetyPoseController avatarSafetyPose_{};
    PerModalityIsolationBroker isolationBroker_{};
    PatientModalityPolicyGate patientPolicyGate_{};
    ExperimentalChemicalChannelGate chemicalGate_{};
    ClosedLoopPerceptionErrorController closedLoopController_{};
    MultisensoryPhaseLockOrchestrator sync_{};
    ModalityExposureLedger exposureLedger_{};
    PerModalityRateAndDosePolicy rateAndDosePolicy_{};
    ChipCapabilityContract capabilityContract_{};
    PerModalitySafetyEnvelope safetyEnvelope_{};
    ChipRealityAnchorHardChannel realityAnchor_{};
    NeuralVectorSlewRateLimiter slewRateLimiter_{};
    NeuralChipSessionSupervisor sessionSupervisor_{};
    NeuralChipAckHandshake ackHandshake_{};
    FrameLifecycleGuard lifecycleGuard_{};
    SafeChipFrameHoldBuffer holdBuffer_{};
    NeuralChipFrameCommitter committer_;
    NeuralChipPipelineDecisionTrace decisionTrace_{};
    NeuralChipReplayRecorder replayRecorder_{};
    PipelineResumeController resumeController_{};
    bool observedEmergencyLatch_ = false;
    NeuralChipSessionState sessionState_{};
    NeuralChipEmergencyLatch emergencyLatch_{};
    NeuralChipOnlyPipelineConfig config_{};
    FullDiveChipFrame lastSubmittedFrame_{};
    bool hasLastSubmittedFrame_ = false;
    AvatarRigCommandFrame lastRigCommand_{};
};

} // namespace dnxr
