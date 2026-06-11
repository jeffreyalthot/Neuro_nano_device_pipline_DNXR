#include "full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.h"
#include "full_dive/chip_pipeline/SensorEncoderOutputContract.h"
#include "full_dive/chip_pipeline/SanitizeFailureFrameFactory.h"
#include "full_dive/chip_pipeline/WorldFrameSanitizer.h"
#include "full_dive/safety/RealityAnchorDegradationModel.h"
#include "neural_chip/DisabledModalityNeutralityValidator.h"

#include <algorithm>

namespace dnxr {

NeuralChipOnlyPipelineConfig NeuralChipOnlyPipelineConfig::simulationSafeDefaults() {
    NeuralChipOnlyPipelineConfig config;
    config.patientMap = PatientSpecificNeuralMap::simulationDefaults();
    config.capabilities.canVisual = false;
    config.capabilities.canAuditory = false;
    config.capabilities.canHaptic = true;
    config.capabilities.canVestibular = false;
    config.capabilities.canProprioceptive = true;
    config.capabilities.canOlfactory = false;
    config.capabilities.canGustatory = false;
    config.capabilities.supportsHardwareCutoff = true;
    config.capabilities.supportsClosedLoopFeedback = true;
    return config;
}

NeuralChipOnlyFullDivePipeline::NeuralChipOnlyFullDivePipeline(NeuralChipBus& chipBus)
    : NeuralChipOnlyFullDivePipeline(chipBus, NeuralChipOnlyPipelineConfig::simulationSafeDefaults()) {}

NeuralChipOnlyFullDivePipeline::NeuralChipOnlyFullDivePipeline(NeuralChipBus& chipBus, const NeuralChipOnlyPipelineConfig& config)
    : chipBus_(chipBus), committer_(chipBus_, lifecycleGuard_, holdBuffer_), emergencyLatchController_(emergencyLatch_, emergencyResetAudit_, resumeController_), config_(config) {
    stateMachine_.start();
    ChipCapabilities capabilities = config_.capabilities;
    capabilities.canVisual = capabilities.canVisual && config_.enableResearchVision;
    capabilities.canAuditory = capabilities.canAuditory && config_.enableResearchAuditory;
    capabilities.canVestibular = capabilities.canVestibular && config_.enableResearchVestibular;
    capabilities.canOlfactory = capabilities.canOlfactory && config_.enableExperimentalChemicalChannels;
    capabilities.canGustatory = capabilities.canGustatory && config_.enableExperimentalChemicalChannels;
    capabilities.supportsHardwareCutoff = true;
    capabilityContract_.setCapabilities(capabilities);
    chemicalGate_.setEnabled(config_.enableExperimentalChemicalChannels);
    sessionState_.chemicalChannelsEverEnabled = config_.enableExperimentalChemicalChannels;
}

PatientSpecificNeuralMap NeuralChipOnlyFullDivePipeline::effectivePatientMap(const NeuralChipOnlyWorldFrame& world) const {
    PatientSpecificNeuralMap map = world.patientMap;
    if (!config_.enableResearchVision) map.visual.enabled = false;
    if (!config_.enableResearchAuditory) map.auditory.enabled = false;
    if (!config_.enableResearchVestibular) map.vestibular.enabled = false;
    if (!config_.enableExperimentalChemicalChannels) {
        map.olfactory.enabled = false;
        map.gustatory.enabled = false;
    }
    return map;
}

void NeuralChipOnlyFullDivePipeline::recordDiff(const char* stage, const FullDiveChipFrame& before, const FullDiveChipFrame& after) {
    auto deltas = FullDiveChipFrameDiff::compare(before, after);
    if (deltas.empty()) {
        decisionTrace_.record({after.frameId, stage, "frame", "", "StageEvaluated", 0.0f, 0.0f, after.safety.aggregateRisk01});
        return;
    }
    for (const auto& delta : deltas) {
        decisionTrace_.record({after.frameId, stage, delta.modality, delta.fieldPath, "FrameFieldAdjusted", delta.before01, delta.after01, std::max(0.0f, delta.delta01)});
    }
}

PipelineStepResult NeuralChipOnlyFullDivePipeline::rejectWithHold(FullDiveChipFrame frame, const char* reason, float ramp01, const ChipReadFrame& read, PipelinePathKind path) {
    currentPathKind_ = path;
    FrameCommitResult result = committer_.commitRejectedWithHold(frame, reason, ramp01);
    decisionTrace_.record({frame.frameId, "NeuralChipFrameCommitter", "commit", result.holdFallback, reason, 1.0f, 0.0f, frame.safety.aggregateRisk01});
    ++sessionState_.rejectedFrames;
    ++sessionState_.holdFrames;
    const bool stillUnsafe = path == PipelinePathKind::AckMissingHold || path == PipelinePathKind::EmergencyLatchHold || path == PipelinePathKind::RealityAnchorHold || path == PipelinePathKind::ValidatorReject;
    holdStabilityTracker_.observeHold(stateMachine_.state(), result.holdFallback, reason, stillUnsafe);
    holdStabilityTracker_.applyTransitions(stateMachine_, stillUnsafe);
    recordReplay(read, frame, result, reason, PipelineStepOutcome::HoldCommitted, false);
    const PipelineStepOutcome outcome = stateMachine_.emergencyLatched() ? PipelineStepOutcome::EmergencyLatched : (result.safetyHold ? PipelineStepOutcome::HoldCommitted : PipelineStepOutcome::Rejected);
    return {outcome, path, stateMachine_.state(), result.accepted, true, reason};
}

void NeuralChipOnlyFullDivePipeline::recordReplay(const ChipReadFrame& read, const FullDiveChipFrame& candidate, const FrameCommitResult& result, const char* holdReason, PipelineStepOutcome outcome, bool noOp) {
    NeuralChipReplayRecord record;
    record.frameId = candidate.frameId;
    record.timestampMs = candidate.timestampMs;
    record.inputWorldSummary = "neural-chip-only step";
    record.readFrame = read;
    record.motorReadFrame = read;
    record.candidateFrame = candidate;
    record.outputChipFrame = result.submittedFrame;
    record.committedOrHoldFrame = result.submittedFrame;
    auto coverage = traceCoverageValidator_.validate(decisionTrace_, currentPathKind_);
    if (!coverage.complete) {
        decisionTrace_.record({candidate.frameId, "DecisionTraceCoverageValidator", "trace", coverage.missingStage, "MissingRequiredStage", 1.0f, 0.0f, 1.0f});
    }
    record.decisionTrace = decisionTrace_.events();
    record.commitResult = result;
    record.noOp = noOp;
    record.stepOutcome = outcome;
    record.traceCoverageComplete = coverage.complete || !strictTraceCoverage_;
    record.traceCoverageMissingStage = coverage.missingStage;
    record.emergencyLatched = emergencyLatch_.latched();
    record.holdReason = holdReason ? holdReason : result.reason;
    record.sessionRisk01 = sessionState_.sessionRisk01;
    record.sessionFatigue01 = sessionState_.sessionFatigue01;
    sessionDigest_.update(record);
    record.sessionDigestHash = sessionDigest_.value().rollingHash;
    record.sessionDigestFrameCount = sessionDigest_.value().frameCount;
    record.pipelinePathKind = currentPathKind_;
    record.pipelineState = stateMachine_.state();
    replayRecorder_.record(record);
}

bool NeuralChipOnlyFullDivePipeline::criticalSafetyViolation(const FullDiveChipFrame& frame, const char** reason) const {
    if (!frame.safety.patientStopSignalAlive) { *reason = "patient stop signal lost"; return true; }
    if (!frame.safety.emergencyCutoffAvailable) { *reason = "emergency cutoff unavailable"; return true; }
    if (!frame.safety.rawSensoryPayloadBlocked) { *reason = "raw sensory payload is not blocked"; return true; }
    if (!frame.safety.painMappingBlocked) { *reason = "pain mapping is not blocked"; return true; }
    if (!realityAnchor_.validates(frame.realityAnchor)) { *reason = "reality anchor hard channel failed"; return true; }
    const auto validation = NeuralVectorValidator::validate(frame);
    if (!validation.accepted) { *reason = validation.reason; return true; }
    return false;
}

void NeuralChipOnlyFullDivePipeline::markAccepted(const FullDiveChipFrame& frame) {
    lastSubmittedFrame_ = frame;
    hasLastSubmittedFrame_ = true;
    ++sessionState_.acceptedFrames;
    sessionState_.sessionRisk01 = sessionState_.sessionRisk01 * 0.99f + frame.safety.aggregateRisk01 * 0.01f;
    sessionState_.motorConfidenceRolling01 = sessionState_.motorConfidenceRolling01 * 0.95f + frame.motorIntent.confidence01 * 0.05f;
    sessionState_.realityAnchorStability01 = std::min({frame.realityAnchor.orientation01, frame.realityAnchor.identityContinuity01,
                                                       frame.realityAnchor.sessionBoundary01, frame.realityAnchor.externalWorldRecall01});
}

bool NeuralChipOnlyFullDivePipeline::step(const NeuralChipOnlyWorldFrame& rawWorld) {
    return stepDetailed(rawWorld).outcome == PipelineStepOutcome::NormalAccepted;
}

PipelineStepResult NeuralChipOnlyFullDivePipeline::finishStep(PipelineStepResult result) {
    updatePipelineStepMetrics(stepMetrics_, result.outcome);
    return result;
}

void NeuralChipOnlyFullDivePipeline::auditTransition(NeuralChipPipelineState before, const char* reason, PipelinePathKind path) {
    transitionAudit_.record(before, stateMachine_.state(), reason, timebase_.nowMs(), path);
}

void NeuralChipOnlyFullDivePipeline::recordNoOpReplay(std::uint64_t frameId, double timestampMs, const char* reason, PipelinePathKind path, PipelineStepOutcome outcome) {
    currentPathKind_ = path;
    FullDiveChipFrame frame{};
    frame.frameId = frameId;
    frame.timestampMs = timestampMs;
    FrameCommitResult result{};
    result.accepted = false;
    result.safetyHold = false;
    result.reason = reason;
    result.submittedFrame = frame;
    decisionTrace_.record({frameId, "NeuralChipPipelineStateGate", "state", toString(stateMachine_.state()), reason, 1.0f, 0.0f, 0.0f});
    if (path == PipelinePathKind::AwaitingAckWait) decisionTrace_.record({frameId, "NeuralChipAckHandshake", "ack", "pending", reason, 1.0f, 0.0f, 0.0f});
    recordReplay({}, frame, result, reason, outcome, true);
}

PipelineStepResult NeuralChipOnlyFullDivePipeline::stepDetailed(const NeuralChipOnlyWorldFrame& rawWorld) {
    decisionTrace_.clear();
    const auto worldSanitization = WorldFrameSanitizer{}.sanitize(rawWorld, config_);
    if (!worldSanitization.accepted) {
        decisionTrace_.record({rawWorld.frameId, "WorldFrameSanitizer", "world", "sanitize", worldSanitization.reason, 1.0f, 0.0f, 0.0f});
        FullDiveChipFrame holdProbe = SanitizeFailureFrameFactory::makeFailureProbe(rawWorld, worldSanitization, rawWorld.frameId, timebase_.nowMs());
        return finishStep(rejectWithHold(holdProbe, worldSanitization.reason, 0.0f, {}, PipelinePathKind::WorldFrameReject));
    }
    const NeuralChipOnlyWorldFrame world = worldSanitization.sanitized;
    timebase_.beginStep(world.timestampMs);
    FrameLifecyclePolicy lifecyclePolicy = lifecycleGuard_.policy();
    lifecyclePolicy.nowMs = timebase_.nowMs();
    lifecycleGuard_.setPolicy(lifecyclePolicy);

    if (emergencyLatch_.latched()) {
        observedEmergencyLatch_ = true;
        stateMachine_.triggerEmergency(emergencyLatch_.reason());
        resumeController_.lock();
    }

    if (ackHandshake_.shouldHold(timebase_.nowMs())) {
        stateMachine_.onAckMissingHold();
        decisionTrace_.record({world.frameId, "NeuralChipAckHandshake", "ack", "timeout", "neural chip ack missing", 1.0f, 0.0f, 0.20f});
        FullDiveChipFrame ackHold = ackHoldFactory_.makeAckHoldFrame(world.frameId, timebase_.nowMs());
        return finishStep(rejectWithHold(ackHold, "neural chip ack missing", 0.20f, {}, PipelinePathKind::AckMissingHold));
    }

    const PipelineStateDecision stateDecision = stateGate_.evaluate(stateMachine_, ackHandshake_, resumeController_, timebase_.nowMs());
    if (stateDecision.waitOnly) {
        recordNoOpReplay(world.frameId, timebase_.nowMs(), stateDecision.reason, stateMachine_.state() == NeuralChipPipelineState::Shutdown ? PipelinePathKind::ShutdownNoOp : PipelinePathKind::AwaitingAckWait, stateMachine_.state() == NeuralChipPipelineState::Shutdown ? PipelineStepOutcome::Shutdown : PipelineStepOutcome::WaitingForAck);
        return finishStep({stateMachine_.state() == NeuralChipPipelineState::Shutdown ? PipelineStepOutcome::Shutdown : PipelineStepOutcome::WaitingForAck, stateMachine_.state() == NeuralChipPipelineState::Shutdown ? PipelinePathKind::ShutdownNoOp : PipelinePathKind::AwaitingAckWait, stateMachine_.state(), false, true, stateDecision.reason});
    }

    if (!stateDecision.allowWorldProcessing) {
        if (stateDecision.holdKind == PipelineHoldKind::Decompression) stateMachine_.onAckMissingHold();
        decisionTrace_.record({world.frameId, "NeuralChipPipelineStateGate", "state", toString(stateMachine_.state()), stateDecision.reason, 1.0f, stateDecision.ramp01, 0.0f});
        if (stateMachine_.awaitingAck()) decisionTrace_.record({world.frameId, "NeuralChipAckHandshake", "ack", "pending", stateDecision.reason, 1.0f, 0.0f, 0.20f});
        if (stateMachine_.emergencyLatched()) decisionTrace_.record({world.frameId, "NeuralChipEmergencyLatch", "latch", "active", stateDecision.reason, 1.0f, 0.0f, 1.0f});
        FullDiveChipFrame holdProbe = ackHoldFactory_.makeStateHoldFrame(stateDecision, world.frameId, timebase_.nowMs());
        const PipelinePathKind path = stateMachine_.emergencyLatched() ? PipelinePathKind::EmergencyLatchHold : (stateMachine_.awaitingAck() || stateDecision.holdKind == PipelineHoldKind::Decompression ? PipelinePathKind::AckMissingHold : PipelinePathKind::ResumeHold);
        return finishStep(rejectWithHold(holdProbe, stateDecision.reason, stateDecision.ramp01, {}, path));
    }

    resumeStateBridge_.synchronizeBeforeDecision(stateMachine_, resumeController_, emergencyLatch_.latched());

    if (observedEmergencyLatch_ && !emergencyLatch_.latched() && resumeController_.locked()) {
        resumeController_.beginControlledResume();
    }

    ChipReadFrame read = chipBus_.readNeuralActivity();
    MotorCortexReadFrame cortexFrame = MotorCortexReadFrameAdapter::fromChipReadFrame(read);
    NeuralMotorIntentVector motor = motorDecoder_.decode(cortexFrame);
    MotorIntentConfidenceDecision motorDecision{};
    motor = motorConfidenceGate_.apply(motor, read.signalQuality01, &motorDecision);
    lastRigCommand_ = avatarRig_.apply(motor);
    if (motorDecision.safetyPoseRequired || emergencyLatch_.latched()) {
        lastRigCommand_ = avatarSafetyPose_.apply(lastRigCommand_, config_.enableResearchVision);
    }

    NeuralVisualVector visual = visualEncoder_.encode(world.scene, world.visualMap, world.visualSafety);
    NeuralAuditoryVector auditory = auditoryEncoder_.encode(world.audio, world.auditoryMap, world.auditorySafety);
    NeuralTouchVector touch = epidermisEncoder_.encode(world.contacts, world.touchMap, world.touchSafety);
    NeuralProprioceptionVector proprio = proprioceptionEncoder_.encode(world.pose);
    NeuralVestibularVector vestibular = vestibularEncoder_.encode(world.motion);
    NeuralOlfactoryVector olfactory = olfactoryEncoder_.encode(world.odor, world.olfactoryMap, world.olfactorySafety);
    NeuralGustatoryVector gustatory = gustatoryEncoder_.encode(world.taste, world.gustatoryMap, world.gustatorySafety);

    SensorEncoderOutputContract encoderContract;
    for (const auto result : {encoderContract.validate(visual), encoderContract.validate(auditory), encoderContract.validate(touch),
                              encoderContract.validate(proprio), encoderContract.validate(vestibular),
                              encoderContract.validate(olfactory, config_.enableExperimentalChemicalChannels),
                              encoderContract.validate(gustatory, config_.enableExperimentalChemicalChannels)}) {
        if (!result.accepted) {
            decisionTrace_.record({world.frameId, "SensorEncoderOutputContract", "encoder", "validate", result.reason, 1.0f, 0.0f, 0.0f});
            FullDiveChipFrame holdProbe{};
            holdProbe.frameId = world.frameId;
            holdProbe.timestampMs = world.timestampMs;
            return finishStep(rejectWithHold(holdProbe, result.reason, 0.0f, read, PipelinePathKind::EarlyEncoderReject));
        }
    }

    FullDiveChipFrame frame = isolationBroker_.assemble(
        visual, auditory, touch, proprio, vestibular, motor, olfactory, gustatory, world.frameId, world.timestampMs);
    frame.realityAnchor = world.realityAnchor;

    if (emergencyLatch_.latched()) {
        observedEmergencyLatch_ = true;
        stateMachine_.triggerEmergency(emergencyLatch_.reason());
        decisionTrace_.record({frame.frameId, "NeuralChipEmergencyLatch", "latch", "active", emergencyLatch_.reason(), 1.0f, 0.0f, frame.safety.aggregateRisk01});
        resumeController_.lock();
        return finishStep(rejectWithHold(frame, emergencyLatch_.reason(), 0.0f, read, PipelinePathKind::EmergencyLatchHold));
    }

    const auto resumeDecision = resumeController_.decide();
    if (!resumeDecision.allowNormalFrame) {
        decisionTrace_.record({frame.frameId, "PipelineResumeController", "resume", "hold", resumeDecision.reason, 1.0f, resumeDecision.globalGain01, frame.safety.aggregateRisk01});
        return finishStep(rejectWithHold(frame, resumeDecision.reason, resumeDecision.globalGain01, read, PipelinePathKind::ResumeHold));
    }
    resumeStateBridge_.synchronizeAfterDecision(stateMachine_, resumeController_, resumeDecision);
    if (resumeDecision.globalGain01 < 1.0f) {
        frame.visual.globalLuminance01 *= resumeDecision.globalGain01;
        frame.auditory.loudness01 *= resumeDecision.globalGain01;
        frame.touch.contactPressure01 *= resumeDecision.globalGain01;
        frame.motorIntent.stopIntent01 = std::max(frame.motorIntent.stopIntent01, 1.0f - resumeDecision.globalGain01);
    }

    FullDiveChipFrame before = frame;
    frame = patientPolicyGate_.apply(frame, effectivePatientMap(world));
    recordDiff("PatientModalityPolicyGate", before, frame);

    before = frame;
    frame = chemicalGate_.apply(frame);
    recordDiff("ExperimentalChemicalChannelGate", before, frame);

    if (hasLastSubmittedFrame_) {
        auto error = closedLoopController_.analyze(lastSubmittedFrame_, read);
        auto correction = closedLoopController_.decide(error);
        before = frame;
        frame = closedLoopController_.applyCorrection(frame, correction);
        recordDiff("ClosedLoopPerceptionErrorController", before, frame);
        sessionState_.closedLoopStability01 = 1.0f - std::max({error.visualError01, error.auditoryError01, error.touchError01,
                                                              error.proprioceptionError01, error.vestibularError01,
                                                              error.olfactoryError01, error.gustatoryError01, error.motorError01});
        if (correction.requireHold) {
            return finishStep(rejectWithHold(frame, "closed-loop perception divergence", 0.20f, read, PipelinePathKind::SessionSupervisorHold));
        }
    } else {
        recordDiff("ClosedLoopPerceptionErrorController", frame, frame);
    }

    PhaseLockDecision syncDecision = sync_.decide(frame, world.latencyBudget);
    if (syncDecision.holdLastSafeFrame) {
        return finishStep(rejectWithHold(frame, "multisensory phase lock failed", 0.35f, read, PipelinePathKind::ValidatorReject));
    }
    before = frame;
    frame = sync_.synchronize(frame, world.latencyBudget);
    recordDiff("MultisensoryPhaseLockOrchestrator", before, frame);

    before = frame;
    frame = exposureLedger_.apply(frame, rateAndDosePolicy_.policy());
    recordDiff("ModalityExposureLedger", before, frame);
    sessionState_.sessionFatigue01 = std::max({exposureLedger_.state().visual.fatigue01, exposureLedger_.state().auditory.fatigue01,
                                               exposureLedger_.state().touch.fatigue01, exposureLedger_.state().vestibular.fatigue01});

    SessionSupervisorDecision supervisorDecision = sessionSupervisor_.decide(sessionState_, frame);
    before = frame;
    frame = sessionSupervisor_.apply(frame, supervisorDecision);
    recordDiff("NeuralChipSessionSupervisor", before, frame);
    if (supervisorDecision.triggerEmergencyLatch) {
        emergencyLatch_.trigger(supervisorDecision.reason);
        stateMachine_.triggerEmergency(supervisorDecision.reason);
        sessionState_.emergencyStopLatched = true;
        lastRigCommand_ = avatarSafetyPose_.apply(lastRigCommand_, config_.enableResearchVision);
        return finishStep(rejectWithHold(frame, supervisorDecision.reason, 0.0f, read, PipelinePathKind::SessionSupervisorHold));
    }
    if (supervisorDecision.requireHold) {
        lastRigCommand_ = avatarSafetyPose_.apply(lastRigCommand_, config_.enableResearchVision);
        return finishStep(rejectWithHold(frame, supervisorDecision.reason, 0.25f, read, PipelinePathKind::SessionSupervisorHold));
    }

    before = frame;
    frame = capabilityContract_.apply(frame);
    recordDiff("ChipCapabilityContract", before, frame);
    if (capabilityContract_.requiresSafetyHold()) {
        return finishStep(rejectWithHold(frame, "chip capability contract requires hardware cutoff", 0.0f, read, PipelinePathKind::CapabilityHold));
    }

    before = frame;
    frame = safetyEnvelope_.apply(frame);
    recordDiff("PerModalitySafetyEnvelope", before, frame);

    ChipCapabilities effectiveCapabilities = config_.capabilities;
    effectiveCapabilities.canVisual = effectiveCapabilities.canVisual && config_.enableResearchVision;
    effectiveCapabilities.canAuditory = effectiveCapabilities.canAuditory && config_.enableResearchAuditory;
    effectiveCapabilities.canVestibular = effectiveCapabilities.canVestibular && config_.enableResearchVestibular;
    effectiveCapabilities.canOlfactory = effectiveCapabilities.canOlfactory && config_.enableExperimentalChemicalChannels;
    effectiveCapabilities.canGustatory = effectiveCapabilities.canGustatory && config_.enableExperimentalChemicalChannels;
    const auto disabledNeutrality = DisabledModalityNeutralityValidator{}.validate(frame, effectiveCapabilities);
    if (!disabledNeutrality.accepted) {
        return finishStep(rejectWithHold(frame, disabledNeutrality.reason, 0.0f, read, PipelinePathKind::CapabilityHold));
    }

    RealityAnchorDegradationModel degradation;
    const auto degradationDecision = degradation.decide(frame.realityAnchor);
    before = frame;
    frame = degradation.apply(frame, degradationDecision);
    recordDiff("RealityAnchorDegradationModel", before, frame);
    if (degradationDecision.triggerEmergencyLatch) {
        emergencyLatch_.trigger(degradationDecision.reason);
        stateMachine_.triggerEmergency(degradationDecision.reason);
        sessionState_.emergencyStopLatched = true;
        observedEmergencyLatch_ = true;
        resumeController_.lock();
        return finishStep(rejectWithHold(frame, degradationDecision.reason, 0.0f, read, PipelinePathKind::EmergencyLatchHold));
    }
    if (degradationDecision.requireHold) {
        return finishStep(rejectWithHold(frame, degradationDecision.reason, degradationDecision.globalGain01, read, PipelinePathKind::RealityAnchorHold));
    }

    before = frame;
    frame = realityAnchor_.enforce(frame);
    recordDiff("ChipRealityAnchorHardChannel", before, frame);

    const auto invariant = safetyInvariantEngine_.validate(frame, sessionState_, ackHandshake_.state(), effectiveCapabilities);
    if (!invariant.accepted) {
        decisionTrace_.record({frame.frameId, "SafetyInvariantEngine", "system", "invariant", invariant.reason, 1.0f, 0.0f, frame.safety.aggregateRisk01});
        if (invariant.triggerEmergencyLatch) {
            emergencyLatch_.trigger(invariant.reason);
            stateMachine_.triggerEmergency(invariant.reason);
            sessionState_.emergencyStopLatched = true;
        }
        if (invariant.requireHold) return finishStep(rejectWithHold(frame, invariant.reason, 0.0f, read, pathClassifier_.classifyRejectReason(invariant.reason)));
    }

    const char* criticalReason = "";
    if (criticalSafetyViolation(frame, &criticalReason)) {
        emergencyLatch_.trigger(criticalReason);
        stateMachine_.triggerEmergency(criticalReason);
        sessionState_.emergencyStopLatched = true;
        return finishStep(rejectWithHold(frame, criticalReason, 0.0f, read, pathClassifier_.classifyRejectReason(criticalReason)));
    }

    before = frame;
    frame = slewRateLimiter_.apply(frame, rateAndDosePolicy_.policy());
    recordDiff("NeuralVectorSlewRateLimiter", before, frame);

    // FrameLifecycleGuardCommitBoundary -> NeuralVectorValidatorCommitBoundary -> NeuralChipBusCommitBoundary
    currentPathKind_ = PipelinePathKind::NormalCommit;
    FrameCommitResult result = committer_.commitCandidate(frame);
    decisionTrace_.record({frame.frameId, "NeuralChipFrameCommitter", "commit", result.holdFallback, result.reason, result.accepted ? 0.0f : 1.0f, result.accepted ? 1.0f : 0.0f, frame.safety.aggregateRisk01});
    if (result.accepted && !result.safetyHold) {
        markAccepted(frame);
        ackHandshake_.expect(frame.frameId, timebase_.nowMs());
        const auto beforeCommitState = stateMachine_.state();
        stateMachine_.onCommitAccepted();
        transitionAudit_.record(beforeCommitState, stateMachine_.state(), "commit accepted; awaiting ack", timebase_.nowMs(), PipelinePathKind::NormalCommit);
        recordReplay(read, frame, result, "", PipelineStepOutcome::NormalAccepted, false);
        return finishStep({PipelineStepOutcome::NormalAccepted, PipelinePathKind::NormalCommit, stateMachine_.state(), true, true, "accepted"});
    }
    ++sessionState_.rejectedFrames;
    if (result.safetyHold) ++sessionState_.holdFrames;
    currentPathKind_ = pathClassifier_.classifyCommitResult(result);
    recordReplay(read, frame, result, result.reason, result.safetyHold ? PipelineStepOutcome::HoldCommitted : PipelineStepOutcome::Rejected, false);
    return finishStep({result.safetyHold ? PipelineStepOutcome::HoldCommitted : PipelineStepOutcome::Rejected, currentPathKind_, stateMachine_.state(), result.accepted, true, result.reason});
}

const AvatarRigCommandFrame& NeuralChipOnlyFullDivePipeline::lastRigCommand() const { return lastRigCommand_; }
const NeuralChipSessionState& NeuralChipOnlyFullDivePipeline::sessionState() const { return sessionState_; }
const NeuralChipPipelineDecisionTrace& NeuralChipOnlyFullDivePipeline::decisionTrace() const { return decisionTrace_; }
const NeuralChipReplayRecorder& NeuralChipOnlyFullDivePipeline::replayRecorder() const { return replayRecorder_; }
const SessionDigest& NeuralChipOnlyFullDivePipeline::sessionDigest() const { return sessionDigest_.value(); }
const NeuralChipPipelineStateMachine& NeuralChipOnlyFullDivePipeline::stateMachine() const { return stateMachine_; }
const PipelineStepMetrics& NeuralChipOnlyFullDivePipeline::stepMetrics() const { return stepMetrics_; }
const PipelineStateTransitionAudit& NeuralChipOnlyFullDivePipeline::transitionAudit() const { return transitionAudit_; }
void NeuralChipOnlyFullDivePipeline::setStrictTraceCoverage(bool enabled) { strictTraceCoverage_ = enabled; }
void NeuralChipOnlyFullDivePipeline::shutdown(const char* reason) {
    const auto before = stateMachine_.state();
    stateMachine_.shutdown(reason ? reason : "pipeline shutdown");
    transitionAudit_.record(before, stateMachine_.state(), reason ? reason : "pipeline shutdown", timebase_.nowMs(), PipelinePathKind::ShutdownNoOp);
}
NeuralChipTimebase& NeuralChipOnlyFullDivePipeline::timebase() { return timebase_; }
NeuralChipEmergencyLatch& NeuralChipOnlyFullDivePipeline::emergencyLatch() { return emergencyLatch_; }
bool NeuralChipOnlyFullDivePipeline::resetEmergencyLatchWithAudit(const EmergencyLatchResetRequest& request) {
    const bool accepted = emergencyLatchController_.resetWithAudit(request, timebase_.nowMs());
    if (accepted) {
        observedEmergencyLatch_ = false;
        const auto beforeResetState = stateMachine_.state();
        stateMachine_.onAuditedReset();
        transitionAudit_.record(beforeResetState, stateMachine_.state(), "audited reset accepted", timebase_.nowMs(), PipelinePathKind::ResumeHold);
        sessionState_.emergencyStopLatched = false;
    } else if (emergencyLatch_.latched()) {
        stateMachine_.triggerEmergency(emergencyLatch_.reason());
    }
    return accepted;
}
void NeuralChipOnlyFullDivePipeline::injectSimulatedAck(std::uint64_t frameId, float quality01) {
    ackHandshake_.receiveAck(frameId, quality01);
    if (!ackHandshake_.state().hasPendingAck) {
        const auto beforeAckState = stateMachine_.state();
        stateMachine_.onAckValid();
        transitionAudit_.record(beforeAckState, stateMachine_.state(), "ack accepted", timebase_.nowMs(), PipelinePathKind::AwaitingAckWait);
    }
}

} // namespace dnxr
