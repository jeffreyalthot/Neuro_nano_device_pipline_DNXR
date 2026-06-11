#include "full_dive/chip_pipeline/NeuralChipOnlyFullDivePipeline.h"
#include "neural_chip/NeuralChipBus.h"
#include <cassert>
using namespace dnxr;
int main() {
    NeuralChipBus bus;
    NeuralChipOnlyFullDivePipeline pipeline(bus);
    NeuralChipOnlyWorldFrame first; first.frameId = 1; first.timestampMs = 1.0;
    pipeline.step(first);
    NeuralChipOnlyWorldFrame second; second.frameId = 2; second.timestampMs = 100.0;
    pipeline.timebase().setSimulatedNowMs(100.0);
    pipeline.step(second);
    assert(pipeline.stateMachine().state() == NeuralChipPipelineState::HoldDecompression || pipeline.sessionState().holdFrames > 0);
    return 0;
}
