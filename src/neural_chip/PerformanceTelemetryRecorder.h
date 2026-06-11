#pragma once
#include "hil/HILLatencyHistogram.h"
#include <map>
#include <string>
namespace dnxr { struct StepTelemetrySample { double stepDurationMs=0.0, worldSanitizeDurationMs=0.0, stateGateDurationMs=0.0, encoderDurationMs=0.0, safetyPipelineDurationMs=0.0, commitDurationMs=0.0, replayDurationMs=0.0; }; class PerformanceTelemetryRecorder { public: void record(const StepTelemetrySample& sample); PercentileStats statsFor(const std::string& field) const; PercentileStats totalStats() const; std::map<std::string, PercentileStats> summarize() const; private: HILLatencyHistogram step_, world_, state_, encoder_, safety_, commit_, replay_; }; }
