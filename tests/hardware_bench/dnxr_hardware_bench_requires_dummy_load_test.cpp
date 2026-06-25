#include "../../src/hardware_bench/HardwareBenchRuntime.h"
#include <cassert>
int main(){ dnxr::RealNeuralChipHAL hal; auto m=dnxr::HardwareBenchSession::dryRunManifest(); m.activeStimulationEnabled=true; dnxr::DummyLoadAdapter d; dnxr::BoundedStimulusCommand c; c.activeOutputEnabled=true; assert(!dnxr::HardwareBenchRuntime{}.sendActiveToDummyLoad(hal,m,d,c).activeOutputSent); }
