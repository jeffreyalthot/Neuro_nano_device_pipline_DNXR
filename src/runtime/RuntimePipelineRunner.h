#pragma once

#include "RuntimeAcquisitionStage.h"
#include "RuntimeCalibrationStage.h"
#include "RuntimeCycleFinalizer.h"
#include "RuntimeFullDiveStage.h"
#include "RuntimeImmersionFrameStage.h"
#include "RuntimeInferenceStage.h"
#include "RuntimeRenderStage.h"
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

    struct RunInput {
        RuntimeStageContext context;
        StageInputs stages;
        SchedulerTickReport& tickReport;
        RuntimeCallback<RuntimeRenderStage::Input(SchedulerTickReport&)> makeRenderStageInput;
        RuntimePipelineSinks sinks;
        RuntimeStageServices* services = nullptr;
    };

    RuntimePipelineFrame run(const RunInput& input) const;

private:
    static std::vector<float> normalizedTelemetryRiskVector(const std::vector<float>& telemetry);
    static void finishCycle(RuntimePipelineFrame& frame,
                            SchedulerTickReport& tickReport,
                            const RuntimeCycleFinalizer::Input& input,
                            bool safeDecision,
                            bool emergencyActive,
                            const std::string& reason,
                            bool emitBudgetWarning = false);
};

} // namespace dnxr
