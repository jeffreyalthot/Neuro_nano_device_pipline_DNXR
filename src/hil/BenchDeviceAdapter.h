#include <cstddef>
#pragma once
namespace dnxr { struct BenchDeviceSample { double ackLatencyMs=0.0; double commitLatencyMs=0.0; double holdLatencyMs=0.0; double emergencyLatchLatencyMs=0.0; double replayWriteLatencyMs=0.0; bool ackTimedOut=false; bool frameDropped=false; bool holdFallback=false; }; class BenchDeviceAdapter { public: BenchDeviceSample sample(std::size_t step) const; }; }
