#pragma once
#include "RuntimeDiagnostics.h"
#include "PipelineStateMachine.h"
#include "../config/RuntimeConfig.h"
#include "../config/RuntimeMode.h"
#include "../performance/RuntimeProfiler.h"
namespace dnxr { class PipelineOrchestrator { public: explicit PipelineOrchestrator(RuntimeConfig config); PipelineStepResult runSyntheticCycle(std::uint64_t cycle); std::string diagnostics() const; private: RuntimeConfig config_; RuntimeMode mode_; PipelineStateMachine state_; RuntimeDiagnostics diagnostics_; RuntimeProfiler profiler_; }; }
