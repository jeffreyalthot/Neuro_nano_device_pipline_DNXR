#include "../../src/hardware/NeuralChipHAL.h"
#include <cassert>
int main(){ dnxr::RealNeuralChipHAL hal; hal.init("bench"); dnxr::BoundedStimulusCommand c; c.activeOutputEnabled=true; c.dummyLoadAttached=true; c.maxOutputCeiling=0.0f; assert(hal.sendBoundedFrame(c)); }
