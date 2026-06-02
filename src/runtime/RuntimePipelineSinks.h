#pragma once

#include "RuntimeCallback.h"
#include "RuntimePipelineFrame.h"
#include "RuntimePipelineNeuralTypes.h"
#include "../utils/RuntimeMetrics.h"

#include <string>
#include <vector>

namespace dnxr {

struct RuntimePipelineSinks {
    RuntimeCallback<void(const RuntimePipelineFrame&)> publishPipelineState;
    RuntimeCallback<void(const RuntimeMetricsSnapshot&)> publishRuntimeMetrics;
    RuntimeCallback<void(const std::vector<float>&, double)> recordReplayFrame;
    RuntimeCallback<void()> triggerEmergency;
    RuntimeCallback<void()> throttleEmergency;
    RuntimeCallback<void()> recordDroppedFrame;
    RuntimeCallback<void(const SafeNeuralStimulusCommand&)> auditRejectedStimulus;
    RuntimeCallback<void(const std::string&)> logWarn;
    RuntimeCallback<void(const std::string&)> logError;
};

} // namespace dnxr
