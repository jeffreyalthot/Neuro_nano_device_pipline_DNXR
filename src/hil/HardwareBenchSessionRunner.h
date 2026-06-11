#pragma once
#include "hil/BenchDeviceAdapter.h"
#include "hil/HILLatencyHistogram.h"
#include <string>
namespace dnxr {
struct HardwareBenchMetrics { PercentileStats ackLatency; PercentileStats commitLatency; PercentileStats holdLatency; PercentileStats emergencyLatchLatency; PercentileStats replayWriteLatency; int droppedFrameCount=0; int ackTimeoutCount=0; int holdFallbackCount=0; int transitionCount=0; bool humanSubjectAllowed=false; bool phantomLoopbackOnly=true; std::string toJson() const; };
class HardwareBenchSessionRunner { public: HardwareBenchMetrics run(std::size_t steps, const BenchDeviceAdapter& adapter = {}) const; bool writeMetricsJson(const std::string& path, const HardwareBenchMetrics& metrics) const; };
}
