#include "full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.h"

#include <cassert>
#include <string>

namespace {

dnxr::NeuralChipOnlyWorldFrame worldFrame(std::uint64_t frameId, double timestampMs) {
    dnxr::NeuralChipOnlyWorldFrame world;
    world.frameId = frameId;
    world.timestampMs = timestampMs;
    world.contacts.pressure01 = 0.20f;
    world.contacts.area01 = 0.20f;
    world.contacts.footGround01 = 0.80f;
    world.pose.handConfidence01 = 0.90f;
    world.pose.footConfidence01 = 0.90f;
    world.pose.ownership01 = 0.90f;
    world.motion.angularVelocity01 = 0.10f;
    world.motion.linearAcceleration01 = 0.10f;
    return world;
}

dnxr::ChipReadFrame motorRead(std::uint64_t frameId, float confidence01, float moveForward01) {
    dnxr::ChipReadFrame read;
    read.frameId = frameId;
    read.signalQuality01 = confidence01;
    read.motorIntent.moveForward01 = moveForward01;
    read.motorIntent.gaitIntent01 = moveForward01;
    read.motorIntent.postureBalance01 = 0.90f;
    read.motorIntent.confidence01 = confidence01;
    read.motorIntent.stopIntent01 = 0.0f;
    return read;
}

} // namespace

int main() {
    dnxr::NeuralChipBus bus;
    dnxr::NeuralChipOnlyFullDivePipeline pipeline(bus);

    bus.injectSimulatedMotorReadFrame(motorRead(10, 0.95f, 0.70f));
    assert(pipeline.step(worldFrame(10, 100.0)));

    const auto command10 = pipeline.lastMotorIntentCommand();
    assert(command10.frameId == 10U);
    assert(command10.validFromFrameId == 11U);
    assert(command10.validUntilFrameId == 11U);
    assert(command10.safetyDisposition == dnxr::MotorIntentSafetyDisposition::Normal);
    assert(command10.confidence01 > 0.90f);
    assert(command10.commandHash != 0U);
    assert(!pipeline.recordedMotorIntentSink().commands().empty());
    assert(bus.lastAcceptedFrame().frameId == 10U);

    pipeline.injectSimulatedAck(10, 1.0f);
    bus.injectSimulatedFeedbackFrame(motorRead(10, 0.92f, 0.65f));

    auto world11 = worldFrame(11, 116.0);
    world11.motion.linearAcceleration01 = command10.intent.moveForward01 * 0.20f;
    bus.injectSimulatedMotorReadFrame(motorRead(11, 0.90f, 0.40f));
    assert(pipeline.step(world11));
    assert(bus.lastAcceptedFrame().frameId == 11U);
    assert(pipeline.sessionState().closedLoopStability01 > 0.0f);

    bool sawFeedback = false;
    bool sawMotorCommand = false;
    for (const auto& event : pipeline.decisionTrace().events()) {
        sawFeedback = sawFeedback || event.stage == "ClosedLoopNeuralFeedbackReader";
        sawMotorCommand = sawMotorCommand || event.stage == "MotorIntentCommand";
        assert(event.stage != "XRRenderer");
        assert(event.stage != "XRHeadsetInterface");
    }
    assert(sawFeedback);
    assert(sawMotorCommand);

    pipeline.injectSimulatedAck(11, 1.0f);
    bus.injectSimulatedFeedbackFrame({});
    bus.injectSimulatedMotorReadFrame(motorRead(12, 0.90f, 0.50f));
    assert(!pipeline.step(worldFrame(12, 132.0)));
    assert(bus.getHealthStatus().safeHoldActive);

    dnxr::NeuralChipBus lowConfidenceBus;
    dnxr::NeuralChipOnlyFullDivePipeline lowConfidencePipeline(lowConfidenceBus);
    lowConfidenceBus.injectSimulatedMotorReadFrame(motorRead(20, 0.10f, 1.0f));
    assert(!lowConfidencePipeline.step(worldFrame(20, 200.0)));
    assert(lowConfidencePipeline.lastMotorIntentCommand().safetyDisposition !=
           dnxr::MotorIntentSafetyDisposition::Normal);
    assert(lowConfidencePipeline.lastRigCommand().footLock01 >= 1.0f);

    return 0;
}
