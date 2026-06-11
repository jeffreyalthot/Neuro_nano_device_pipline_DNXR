#include "full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.h"

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
    : chipBus_(chipBus), committer_(chipBus_, lifecycleGuard_, holdBuffer_), config_(config) {
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
    for (const auto& delta : FullDiveChipFrameDiff::compare(before, after)) {
        decisionTrace_.record({after.frameId, stage, delta.fieldPath, "FrameFieldAdjusted", delta.before01, delta.after01, std::max(0.0f, delta.delta01)});
    }
}

bool NeuralChipOnlyFullDivePipeline::rejectWithHold(FullDiveChipFrame frame, const char* reason, float ramp01, const ChipReadFrame& read) {
    FrameCommitResult result = committer_.commitRejectedWithHold(frame, reason, ramp01);
    decisionTrace_.record({frame.frameId, "Commit", result.holdFallback, reason, 1.0f, 0.0f, frame.safety.aggregateRisk01});
    ++sessionState_.rejectedFrames;
    ++sessionState_.holdFrames;
    recordReplay(read, frame, result, reason);
    return result.accepted && !result.safetyHold;
}

void NeuralChipOnlyFullDivePipeline::recordReplay(const ChipReadFrame& read, const FullDiveChipFrame& candidate, const FrameCommitResult& result, const char* holdReason) {
    NeuralChipReplayRecord record;
    record.frameId = candidate.frameId;
    record.timestampMs = candidate.timestampMs;
    record.inputWorldSummary = "neural-chip-only step";
    record.readFrame = read;
    record.motorReadFrame = read;
    record.candidateFrame = candidate;
    record.outputChipFrame = result.submittedFrame;
    record.committedOrHoldFrame = result.submittedFrame;
    record.decisionTrace = decisionTrace_.events();
    record.commitResult = result;
    record.emergencyLatched = emergencyLatch_.latched();
    record.holdReason = holdReason ? holdReason : result.reason;
    record.sessionRisk01 = sessionState_.sessionRisk01;
    record.sessionFatigue01 = sessionState_.sessionFatigue01;
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

bool NeuralChipOnlyFullDivePipeline::step(const NeuralChipOnlyWorldFrame& world) {
    decisionTrace_.clear();

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

    FullDiveChipFrame frame = isolationBroker_.assemble(
        visual, auditory, touch, proprio, vestibular, motor, olfactory, gustatory, world.frameId, world.timestampMs);
    frame.realityAnchor = world.realityAnchor;

    if (emergencyLatch_.latched()) {
        return rejectWithHold(frame, emergencyLatch_.reason(), 0.0f, read);
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
            return rejectWithHold(frame, "closed-loop perception divergence", 0.20f, read);
        }
    }

    PhaseLockDecision syncDecision = sync_.decide(frame, world.latencyBudget);
    if (syncDecision.holdLastSafeFrame) {
        return rejectWithHold(frame, "multisensory phase lock failed", 0.35f, read);
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
        sessionState_.emergencyStopLatched = true;
        lastRigCommand_ = avatarSafetyPose_.apply(lastRigCommand_, config_.enableResearchVision);
        return rejectWithHold(frame, supervisorDecision.reason, 0.0f, read);
    }
    if (supervisorDecision.requireHold) {
        lastRigCommand_ = avatarSafetyPose_.apply(lastRigCommand_, config_.enableResearchVision);
        return rejectWithHold(frame, supervisorDecision.reason, 0.25f, read);
    }

    before = frame;
    frame = capabilityContract_.apply(frame);
    recordDiff("ChipCapabilityContract", before, frame);
    if (capabilityContract_.requiresSafetyHold()) {
        return rejectWithHold(frame, "chip capability contract requires hardware cutoff", 0.0f, read);
    }

    before = frame;
    frame = safetyEnvelope_.apply(frame);
    recordDiff("PerModalitySafetyEnvelope", before, frame);

    before = frame;
    frame = realityAnchor_.enforce(frame);
    recordDiff("ChipRealityAnchorHardChannel", before, frame);

    const char* criticalReason = "";
    if (criticalSafetyViolation(frame, &criticalReason)) {
        emergencyLatch_.trigger(criticalReason);
        sessionState_.emergencyStopLatched = true;
        return rejectWithHold(frame, criticalReason, 0.0f, read);
    }

    before = frame;
    frame = slewRateLimiter_.apply(frame, rateAndDosePolicy_.policy());
    recordDiff("NeuralVectorSlewRateLimiter", before, frame);

    if (ackHandshake_.shouldHold(world.timestampMs)) {
        return rejectWithHold(frame, "neural chip ack missing", 0.20f, read);
    }

    // FrameLifecycleGuardCommitBoundary -> NeuralVectorValidatorCommitBoundary -> NeuralChipBusCommitBoundary
    FrameCommitResult result = committer_.commitCandidate(frame);
    if (result.accepted && !result.safetyHold) {
        markAccepted(frame);
        ackHandshake_.expect(frame.frameId, frame.timestampMs);
        ackHandshake_.receiveAck(frame.frameId, 1.0f);
        recordReplay(read, frame, result, "");
        return true;
    }
    ++sessionState_.rejectedFrames;
    if (result.safetyHold) ++sessionState_.holdFrames;
    recordReplay(read, frame, result, result.reason);
    return false;
}

const AvatarRigCommandFrame& NeuralChipOnlyFullDivePipeline::lastRigCommand() const { return lastRigCommand_; }
const NeuralChipSessionState& NeuralChipOnlyFullDivePipeline::sessionState() const { return sessionState_; }
const NeuralChipPipelineDecisionTrace& NeuralChipOnlyFullDivePipeline::decisionTrace() const { return decisionTrace_; }
const NeuralChipReplayRecorder& NeuralChipOnlyFullDivePipeline::replayRecorder() const { return replayRecorder_; }
NeuralChipEmergencyLatch& NeuralChipOnlyFullDivePipeline::emergencyLatch() { return emergencyLatch_; }

} // namespace dnxr
