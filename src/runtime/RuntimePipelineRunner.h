#pragma once

#include "RuntimeAcquisitionStage.h"
#include "RuntimeCalibrationStage.h"
#include "RuntimeCycleFinalizer.h"
#include "RuntimeFullDiveStage.h"
#include "RuntimeFiveSenseCoherenceStage.h"
#include "RuntimeBodySchemaStage.h"
#include "RuntimeClosedLoopFeedbackStage.h"
#include "RuntimePerceptionCorrectionStage.h"
#include "RuntimeImmersionFrameStage.h"
#include "RuntimeInferenceStage.h"
#include "RuntimeRenderStage.h"
#include "RuntimePipelineSinkRefs.h"
#include "RuntimePipelineSinks.h"
#include "RuntimeSafetyStage.h"
#include "RuntimeSignalStage.h"
#include "RuntimeStimulusStage.h"
#include "../core/NeuroRealtimeScheduler.h"
#include "../utils/RuntimeMetrics.h"

#include <chrono>
#include <cstdint>
#include <string>
#include <vector>

namespace dnxr {

struct RuntimeStageServices;

class RuntimePipelineRunner {
public:
    struct StageInputs {
        RuntimeAcquisitionStage::Input acquisition;
        RuntimeSignalStage::Input signal;
        RuntimeInferenceStage::Input inference;
        RuntimeCalibrationStage::Input calibration;
        RuntimeImmersionFrameStage::Input immersionFrame;
        RuntimeSafetyStage::Input safety;
        RuntimeFullDiveStage::Input fullDive;
        RuntimeStimulusStage::Input stimulus;
        RuntimeCycleFinalizer::Input finalizer;
    };

    struct RuntimePipelineRunInput {
        RuntimeStageContext context;
        SchedulerTickReport& tickReport;
        RuntimeStageServices& services;
        RuntimePipelineSinks& sinks;
        RuntimeMetricsSnapshot runtimeMetricsSnapshot;
        RuntimeCycleFinalizer::Context finalizer;
    };

    struct RuntimePipelineRunInputRef {
        RuntimeStageContext context;
        SchedulerTickReport& tickReport;
        RuntimeStageServices& services;
        RuntimePipelineSinkRefs sinks;
        RuntimeMetricsSnapshot runtimeMetricsSnapshot;
        RuntimeCycleFinalizer::Context finalizer;
    };

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
