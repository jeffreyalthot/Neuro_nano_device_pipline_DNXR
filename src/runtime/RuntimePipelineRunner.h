#pragma once

#include "RuntimeAcquisitionStage.h"
#include "RuntimeCalibrationStage.h"
#include "RuntimeCycleFinalizer.h"
#include "RuntimeFullDiveStage.h"
#include "RuntimeImmersionFrameStage.h"
#include "RuntimeInferenceStage.h"
#include "RuntimeRenderStage.h"
#include "RuntimeSafetyStage.h"
#include "RuntimeSignalStage.h"
#include "RuntimeStimulusStage.h"
#include "../core/NeuroRealtimeScheduler.h"
#include "../utils/RuntimeMetrics.h"

#include <chrono>
#include <cstdint>
#include <functional>
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
        std::function<RuntimeRenderStage::Input(SchedulerTickReport&)> makeRenderStageInput;
        std::function<void(const RuntimePipelineFrame&)> publishPipelineState;
        std::function<void(const std::vector<float>&, double)> recordReplayFrame;
        std::function<void()> triggerEmergency;
        std::function<void()> throttleEmergency;
        std::function<void(const std::string&)> logWarn;
        std::function<void(const std::string&)> logError;
        std::function<void()> recordDroppedFrame;
        std::function<void(const SafeNeuralStimulusCommand&)> auditRejectedStimulus;
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
