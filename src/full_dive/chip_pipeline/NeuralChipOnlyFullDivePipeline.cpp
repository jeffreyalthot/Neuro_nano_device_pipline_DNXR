#include "full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.h"

namespace dnxr {

NeuralChipOnlyFullDivePipeline::NeuralChipOnlyFullDivePipeline(NeuralChipBus& chipBus)
    : chipBus_(chipBus) {
    ChipCapabilities capabilities;
    capabilities.canVisual = true;
    capabilities.canAuditory = true;
    capabilities.canHaptic = true;
    capabilities.canVestibular = true;
    capabilities.canProprioceptive = true;
    capabilities.canOlfactory = true;
    capabilities.canGustatory = true;
    capabilities.supportsHardwareCutoff = true;
    capabilities.supportsClosedLoopFeedback = true;
    capabilityContract_.setCapabilities(capabilities);
}

bool NeuralChipOnlyFullDivePipeline::step(const NeuralChipOnlyWorldFrame& world) {
    ChipReadFrame read = chipBus_.readNeuralActivity();
    MotorCortexReadFrame cortexFrame = MotorCortexReadFrameAdapter::fromChipReadFrame(read);
    NeuralMotorIntentVector motor = motorDecoder_.decode(cortexFrame);
    lastRigCommand_ = avatarRig_.apply(motor);

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

    frame = patientPolicyGate_.apply(frame, world.patientMap);
    PhaseLockDecision syncDecision = sync_.decide(frame, world.latencyBudget);
    if (syncDecision.holdLastSafeFrame) {
        FullDiveChipFrame hold = holdBuffer_.makeDecompressionFrame(frame.frameId, frame.timestampMs, 0.35f);
        chipBus_.submitSensoryFrame(hold);
        chipBus_.submitSafetyHold({frame.frameId, true, "multisensory phase lock failed"});
        return false;
    }
    frame = sync_.synchronize(frame, world.latencyBudget);
    frame = exposureLedger_.apply(frame);
    frame = capabilityContract_.apply(frame);
    if (capabilityContract_.requiresSafetyHold()) {
        chipBus_.submitSafetyHold({frame.frameId, true, "chip capability contract requires hardware cutoff"});
        return false;
    }
    frame = safetyEnvelope_.apply(frame);
    frame = realityAnchor_.enforce(frame);

    if (!realityAnchor_.validates(frame.realityAnchor)) {
        chipBus_.submitSafetyHold({frame.frameId, true, "reality anchor hard channel failed"});
        return false;
    }
    auto lifecycle = lifecycleGuard_.validate(frame);
    if (!lifecycle.accepted) {
        FullDiveChipFrame hold = holdBuffer_.makeNeutralHoldFrame(frame.frameId, frame.timestampMs);
        chipBus_.submitSensoryFrame(hold);
        chipBus_.submitSafetyHold({frame.frameId, true, lifecycle.reason});
        return false;
    }
    if (!NeuralVectorValidator::validate(frame).accepted) {
        FullDiveChipFrame hold = holdBuffer_.makeDecompressionFrame(frame.frameId, frame.timestampMs, 0.25f);
        chipBus_.submitSensoryFrame(hold);
        chipBus_.submitSafetyHold({frame.frameId, true, "neural vector validation failed"});
        return false;
    }
    const bool accepted = chipBus_.submitSensoryFrame(frame);
    if (accepted) {
        holdBuffer_.rememberAccepted(frame);
    }
    return accepted;
}

const AvatarRigCommandFrame& NeuralChipOnlyFullDivePipeline::lastRigCommand() const {
    return lastRigCommand_;
}

} // namespace dnxr
