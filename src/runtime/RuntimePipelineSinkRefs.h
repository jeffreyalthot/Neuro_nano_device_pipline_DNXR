#pragma once

#include "RuntimeCallbackRef.h"
#include "RuntimePipelineFrame.h"
#include "RuntimePipelineNeuralTypes.h"
#include "RuntimePipelineSinks.h"
#include "../utils/RuntimeMetrics.h"

#include <string>
#include <vector>

namespace dnxr {

struct RuntimePipelineSinkRefs {
    RuntimeCallbackRef<void(const RuntimePipelineFrame&)> publishPipelineState;
    RuntimeCallbackRef<void(const RuntimeMetricsSnapshot&)> publishRuntimeMetrics;
    RuntimeCallbackRef<void(const std::vector<float>&, double)> recordReplayFrame;
    RuntimeCallbackRef<void()> triggerEmergency;
    RuntimeCallbackRef<void()> throttleEmergency;
    RuntimeCallbackRef<void()> recordDroppedFrame;
    RuntimeCallbackRef<void(const SafeNeuralStimulusCommand&)> auditRejectedStimulus;
    RuntimeCallbackRef<void(const std::string&)> logWarn;
    RuntimeCallbackRef<void(const std::string&)> logError;

    static RuntimePipelineSinkRefs fromSinks(RuntimePipelineSinks& sinks) {
        RuntimePipelineSinkRefs refs;
        if (sinks.publishPipelineState) {
            refs.publishPipelineState = RuntimeCallbackRef<void(const RuntimePipelineFrame&)>(sinks.publishPipelineState);
        }
        if (sinks.publishRuntimeMetrics) {
            refs.publishRuntimeMetrics = RuntimeCallbackRef<void(const RuntimeMetricsSnapshot&)>(sinks.publishRuntimeMetrics);
        }
        if (sinks.recordReplayFrame) {
            refs.recordReplayFrame = RuntimeCallbackRef<void(const std::vector<float>&, double)>(sinks.recordReplayFrame);
        }
        if (sinks.triggerEmergency) {
            refs.triggerEmergency = RuntimeCallbackRef<void()>(sinks.triggerEmergency);
        }
        if (sinks.throttleEmergency) {
            refs.throttleEmergency = RuntimeCallbackRef<void()>(sinks.throttleEmergency);
        }
        if (sinks.recordDroppedFrame) {
            refs.recordDroppedFrame = RuntimeCallbackRef<void()>(sinks.recordDroppedFrame);
        }
        if (sinks.auditRejectedStimulus) {
            refs.auditRejectedStimulus = RuntimeCallbackRef<void(const SafeNeuralStimulusCommand&)>(sinks.auditRejectedStimulus);
        }
        if (sinks.logWarn) {
            refs.logWarn = RuntimeCallbackRef<void(const std::string&)>(sinks.logWarn);
        }
        if (sinks.logError) {
            refs.logError = RuntimeCallbackRef<void(const std::string&)>(sinks.logError);
        }
        return refs;
    }
};

} // namespace dnxr
