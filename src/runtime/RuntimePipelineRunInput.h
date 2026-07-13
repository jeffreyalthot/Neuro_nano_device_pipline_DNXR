#pragma once

#include "RuntimeAcquisitionStage.h"
#include "RuntimeCalibrationStage.h"
#include "RuntimeCycleFinalizer.h"
#include "RuntimeFullDiveStage.h"
#include "RuntimeImmersionFrameStage.h"
#include "RuntimeInferenceStage.h"
#include "RuntimePipelineSinkRefs.h"
#include "RuntimePipelineSinks.h"
#include "RuntimeSafetyStage.h"
#include "RuntimeSignalStage.h"
#include "RuntimeStimulusStage.h"
#include "../core/NeuroRealtimeScheduler.h"
#include "../utils/RuntimeMetrics.h"

namespace dnxr {

struct RuntimePipelineStageInputs {
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

} // namespace dnxr
