#pragma once
#include "HardwareBenchSession.h"
#include "HardwareBenchSelfTest.h"
#include "../bench/DummyLoadAdapter.h"
#include "../hardware/NeuralChipHAL.h"
#include "../real/RealReadinessGate.h"
namespace dnxr {
struct HardwareBenchRuntimeResult { bool started=false; bool dryRun=true; bool activeOutputSent=false; std::string reason; };
class HardwareBenchRuntime { public: HardwareBenchRuntimeResult startDryRun(RealNeuralChipHAL& hal, const HardwareBenchSessionManifest& manifest); HardwareBenchRuntimeResult sendActiveToDummyLoad(RealNeuralChipHAL& hal, const HardwareBenchSessionManifest& manifest, const DummyLoadAdapter& dummyLoad, const BoundedStimulusCommand& command); };
}
