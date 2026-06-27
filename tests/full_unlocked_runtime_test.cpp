#include "core/PipelineOrchestrator.h"
#include <cassert>
int main(){ dnxr::RuntimeConfig c; c.runtimeMode="full_unlocked_mode"; c.simulationMode=true; dnxr::PipelineOrchestrator o(c); auto r=o.runSyntheticCycle(0); assert(r.accepted); assert(o.diagnostics().find("synthetic_cycle_0")!=std::string::npos); return 0; }
