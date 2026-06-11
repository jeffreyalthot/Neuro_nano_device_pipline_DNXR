#include "full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.h"
#include <cassert>

int main() {
    dnxr::NeuralChipBus bus;
    dnxr::FullDiveChipFrame seed;
    seed.frameId = 1;
    seed.motorIntent.moveForward01 = 0.5f;
    seed.motorIntent.gaitIntent01 = 0.4f;
    seed.motorIntent.confidence01 = 0.9f;
    seed.motorIntent.stopIntent01 = 0.0f;
    assert(bus.submitSensoryFrame(seed));

    dnxr::NeuralChipOnlyFullDivePipeline pipeline(bus);
    dnxr::NeuralChipOnlyWorldFrame world;
    world.frameId = 2;
    world.scene.luminance01 = 0.6f;
    world.audio.loudness01 = 0.5f;
    world.contacts.pressure01 = 0.4f;
    world.pose.ownership01 = 0.8f;
    world.motion.angularVelocity01 = 0.2f;
    assert(pipeline.step(world));
    assert(bus.lastAcceptedFrame().frameId == 2);
    assert(bus.lastAcceptedFrame().visual.globalLuminance01 == 0.0f);
    assert(!pipeline.lastRigCommand().joints.empty());
    pipeline.injectSimulatedAck(2, 1.0f);

    world.frameId = 3;
    world.realityAnchor.orientation01 = 0.1f;
    assert(!pipeline.step(world));
    assert(bus.getHealthStatus().safeHoldActive);
    return 0;
}
