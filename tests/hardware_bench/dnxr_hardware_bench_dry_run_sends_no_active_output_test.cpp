#include "../../src/hardware_bench/HardwareBenchRuntime.h"
#include <cassert>
int main(){ dnxr::RealNeuralChipHAL hal; auto r=dnxr::HardwareBenchRuntime{}.startDryRun(hal,dnxr::HardwareBenchSession::dryRunManifest()); assert(r.started); assert(r.dryRun); assert(!r.activeOutputSent); }
