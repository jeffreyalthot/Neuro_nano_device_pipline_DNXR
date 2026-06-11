#include "full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.h"

namespace dnxr {

NeuralChipOnlyFullDivePipeline::NeuralChipOnlyFullDivePipeline(NeuralChipBus& chipBus)
    : chipBus_(chipBus) {}

bool NeuralChipOnlyFullDivePipeline::step(const NeuralChipOnlyWorldFrame& world) {
    ChipReadFrame read = chipBus_.readNeuralActivity();
    NeuralMotorIntentVector motor = read.motorIntent;
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

    frame = sync_.synchronize(frame);
    frame = exposureLedger_.apply(frame);
    frame = safetyEnvelope_.apply(frame);
    frame = realityAnchor_.enforce(frame);

    if (!realityAnchor_.validates(frame.realityAnchor)) {
        chipBus_.submitSafetyHold({frame.frameId, true, "reality anchor hard channel failed"});
        return false;
    }
    if (!NeuralVectorValidator::validate(frame).accepted) {
        chipBus_.submitSafetyHold({frame.frameId, true, "neural vector validation failed"});
        return false;
    }
    return chipBus_.submitSensoryFrame(frame);
}

const AvatarRigCommandFrame& NeuralChipOnlyFullDivePipeline::lastRigCommand() const {
    return lastRigCommand_;
}

} // namespace dnxr
