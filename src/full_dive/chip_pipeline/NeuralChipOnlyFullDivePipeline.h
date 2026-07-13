#pragma once

#include "full_dive/auditory_chip/AuditoryTonotopicEncoder.h"
#include "full_dive/chip_pipeline/ClosedLoopPerceptionErrorController.h"
#include "full_dive/gustatory_chip/NeuralGustatoryEncoder.h"
#include "full_dive/motor_chip/AvatarKinematicRigController.h"
#include "full_dive/motor_chip/MotorCortexReadFrameAdapter.h"
#include "full_dive/motor_chip/MotorIntentConfidenceGate.h"
#include "full_dive/motor_chip/MotorIntentCommand.h"
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
#include "full_dive/visual_chip/PhospheneFrameStabilizer.h"
#include "full_dive/visual_chip/VisualCorticalWorldEncoder.h"
#include "full_dive/visual_chip/VisualSceneSemanticExtractor.h"
#include "neural_chip/ChipCapabilityContract.h"
#include "neural_chip/FrameLifecycleGuard.h"
#include "neural_chip/FullDiveChipFrameDiff.h"
#include "neural_chip/NeuralChipBus.h"
#include "neural_chip/NeuralChipAckHandshake.h"
#include "neural_chip/NeuralChipPipelineStateGate.h"
#include "neural_chip/PipelineResumeStateBridge.h"
#include "neural_chip/PipelinePathClassifier.h"
#include "neural_chip/HoldStabilityTracker.h"
#include "neural_chip/EmergencyLatchController.h"
#include "neural_chip/DecisionTraceCoverageValidator.h"
#include "neural_chip/AckHoldFrameFactory.h"
#include "neural_chip/NeuralChipEmergencyLatch.h"
#include "neural_chip/NeuralChipFrameCommitter.h"
#include "neural_chip/NeuralChipPipelineDecisionTrace.h"
#include "neural_chip/PipelineResumeController.h"
#include "neural_chip/NeuralChipSessionState.h"
#include "neural_chip/PipelineStepMetrics.h"
#include "neural_chip/PipelineStateTransitionAudit.h"
#include "neural_chip/NeuralChipSessionSupervisor.h"
#include "neural_chip/NeuralChipReplayRecorder.h"
#include "neural_chip/NeuralChipSessionDigest.h"
#include "neural_chip/NeuralChipPipelineStateMachine.h"
#include "neural_chip/NeuralChipTimebase.h"
#include "neural_chip/PipelineReentrancyGuard.h"
#include "neural_chip/SafetyInvariantEngine.h"
#include "neural_chip/NeuralVectorSlewRateLimiter.h"
#include "neural_chip/NeuralVectorValidator.h"
#include "neural_chip/PatientModalityPolicyGate.h"
#include "neural_chip/PatientSpecificNeuralMap.h"
#include "full_dive/spike_bridge/StimulusBySpikeRuntime.h"
#include "neural_chip/SafeChipFrameHoldBuffer.h"

namespace dnxr {

struct NeuralChipOnlyPipelineConfig {
    ChipCapabilities capabilities{};
#if DNXR_SIMULATION_ONLY
    PatientSpecificNeuralMap patientMap = PatientSpecificNeuralMap::simulationDefaults();
#else
    PatientSpecificNeuralMap patientMap{};
#endif
    bool enableExperimentalChemicalChannels = false;
    bool enableResearchVision = false;
    bool enableResearchAuditory = false;
    bool enableResearchVestibular = false;
    bool simulationOnly = true;
    bool allowStimulusDispatch = false;
    bool experimentalAcknowledgement = false;
    bool researchModeAcknowledgement = false;
    bool strictConfig = false;
    DeploymentSafetyEvidence deploymentSafety{};

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
#if DNXR_SIMULATION_ONLY
    PatientSpecificNeuralMap patientMap = PatientSpecificNeuralMap::simulationDefaults();
#else
    PatientSpecificNeuralMap patientMap{};
#endif
    NeuralChipLatencyBudget latencyBudget;
};

class NeuralChipOnlyFullDivePipeline {
public:
    explicit NeuralChipOnlyFullDivePipeline(NeuralChipBus& chipBus);
    NeuralChipOnlyFullDivePipeline(NeuralChipBus& chipBus, const NeuralChipOnlyPipelineConfig& config);

    bool step(const NeuralChipOnlyWorldFrame& world);
    PipelineStepResult stepDetailed(const NeuralChipOnlyWorldFrame& world);
    const AvatarRigCommandFrame& lastRigCommand() const;
    const MotorIntentCommand& lastMotorIntentCommand() const;
    const RecordingMotorIntentSink& recordedMotorIntentSink() const;
    void setMotorIntentSink(ICanonicalMotorIntentSink* sink);
    const NeuralChipSessionState& sessionState() const;
    const NeuralChipPipelineDecisionTrace& decisionTrace() const;
    const NeuralChipReplayRecorder& replayRecorder() const;
    const SessionDigest& sessionDigest() const;
    const NeuralChipPipelineStateMachine& stateMachine() const;
    const PipelineStepMetrics& stepMetrics() const;
    const PipelineStateTransitionAudit& transitionAudit() const;
    void setStrictTraceCoverage(bool enabled);
    void shutdown(const char* reason);
    NeuralChipTimebase& timebase();
    const NeuralChipEmergencyLatch& emergencyLatchStatus() const;
    void injectSimulatedEmergencyLatchForTest(const char* reason);
    bool resetEmergencyLatchWithAudit(const EmergencyLatchResetRequest& request);
    void injectSimulatedAck(std::uint64_t frameId, float quality01);

private:
    void recordDiff(const char* stage, const FullDiveChipFrame& before, const FullDiveChipFrame& after);
    PipelineStepResult rejectWithHold(FullDiveChipFrame frame, const char* reason, float ramp01, const ChipReadFrame& read, PipelinePathKind path);
    void recordReplay(const ChipReadFrame& read, const FullDiveChipFrame& candidate, const FrameCommitResult& result, const char* holdReason, PipelineStepOutcome outcome = PipelineStepOutcome::Rejected, bool noOp = false);
    void recordNoOpReplay(std::uint64_t frameId, double timestampMs, const char* reason, PipelinePathKind path, PipelineStepOutcome outcome);
    void auditTransition(NeuralChipPipelineState before, const char* reason, PipelinePathKind path);
    PipelineStepResult finishStep(PipelineStepResult result);
    bool criticalSafetyViolation(const FullDiveChipFrame& frame, const char** reason) const;
    void markAccepted(const FullDiveChipFrame& frame);
    MotorIntentCommand buildMotorIntentCommand(std::uint64_t frameId, double timestampMs, const NeuralMotorIntentVector& motor, const MotorIntentConfidenceDecision& decision, const AvatarRigCommandFrame& rig) const;
    bool feedbackUsableForNextFrame(const ChipReadFrame& feedback, std::uint64_t expectedFrameId) const;
    PatientSpecificNeuralMap effectivePatientMap(const NeuralChipOnlyWorldFrame& world) const;

    NeuralChipBus& chipBus_;
    VisualSceneSemanticExtractor visualSemanticExtractor_{};
    VisualCorticalWorldEncoder visualEncoder_{};
    PhospheneFrameStabilizer phospheneStabilizer_{};
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
    StimulusBySpikeRuntime stimulusBySpikeRuntime_{};
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
    NeuralChipSessionDigest sessionDigest_{};
    NeuralChipPipelineStateMachine stateMachine_{};
    NeuralChipTimebase timebase_{};
    SafetyInvariantEngine safetyInvariantEngine_{};
    PipelineResumeController resumeController_{};
    NeuralChipPipelineStateGate stateGate_{};
    AckHoldFrameFactory ackHoldFactory_{};
    PipelinePathClassifier pathClassifier_{};
    DecisionTraceCoverageValidator traceCoverageValidator_{};
    PipelineStepMetrics stepMetrics_{};
    PipelineStateTransitionAudit transitionAudit_{};
    HoldStabilityTracker holdStabilityTracker_{};
    PipelineResumeStateBridge resumeStateBridge_{};
    PipelinePathKind currentPathKind_ = PipelinePathKind::NormalCommit;
    bool strictTraceCoverage_ = false;
    EmergencyResetAuditTrail emergencyResetAudit_{};
    NeuralChipEmergencyLatch emergencyLatch_{};
    EmergencyLatchController emergencyLatchController_;
    bool observedEmergencyLatch_ = false;
    NeuralChipSessionState sessionState_{};
    NeuralChipOnlyPipelineConfig config_{};
    FullDiveChipFrame lastSubmittedFrame_{};
    bool hasLastSubmittedFrame_ = false;
    PipelineReentrancyGuard reentrancyGuard_{};
    AvatarRigCommandFrame lastRigCommand_{};
    MotorIntentCommand lastMotorIntentCommand_{};
    RecordingMotorIntentSink defaultMotorIntentSink_{};
    ICanonicalMotorIntentSink* motorIntentSink_ = nullptr;
};

} // namespace dnxr
