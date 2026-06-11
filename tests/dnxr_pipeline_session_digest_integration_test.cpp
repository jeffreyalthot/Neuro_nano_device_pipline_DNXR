#include "full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.h"
#include "neural_chip/NeuralChipBus.h"
#include <cassert>
using namespace dnxr;
int main() {
    NeuralChipBus bus;
    NeuralChipOnlyFullDivePipeline pipeline(bus);
    NeuralChipOnlyWorldFrame world;
    world.frameId = 1;
    world.timestampMs = 1.0;
    const bool accepted = pipeline.step(world);
    (void)accepted;
    assert(pipeline.sessionDigest().frameCount == pipeline.replayRecorder().records().size());
    assert(pipeline.sessionDigest().frameCount > 0);
    assert(pipeline.replayRecorder().records().back().sessionDigestHash == pipeline.sessionDigest().rollingHash);
    return 0;
}
