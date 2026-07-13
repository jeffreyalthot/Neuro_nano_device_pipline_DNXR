#pragma once

#include "RuntimePipelineRunInput.h"

#include <string>
#include <vector>

namespace dnxr {

struct RuntimeStageServices;

class RuntimePipelineRunner {
public:
    using StageInputs = RuntimePipelineStageInputs;
    using RuntimePipelineRunInput = dnxr::RuntimePipelineRunInput;
    using RuntimePipelineRunInputRef = dnxr::RuntimePipelineRunInputRef;

    RuntimePipelineFrame run(const RuntimePipelineRunInput& input) const;
    RuntimePipelineFrame run(const RuntimePipelineRunInputRef& input) const;

private:
    static std::vector<float> normalizedTelemetryRiskVector(const std::vector<float>& telemetry);
    static void finishModernCycle(RuntimePipelineFrame& frame,
                                  SchedulerTickReport& tickReport,
                                  RuntimeStageServices& services,
                                  RuntimePipelineSinkRefs sinks,
                                  const RuntimeCycleFinalizer::Context& finalizer,
                                  bool safeDecision,
                                  bool emergencyActive,
                                  const std::string& reason,
                                  bool emitBudgetWarning = false);
};

} // namespace dnxr
