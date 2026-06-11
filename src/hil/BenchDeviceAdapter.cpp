#include "hil/BenchDeviceAdapter.h"
#include <cstddef>
namespace dnxr { BenchDeviceSample BenchDeviceAdapter::sample(std::size_t step) const { BenchDeviceSample s; const double base=1.0+static_cast<double>(step%7); s.ackLatencyMs=base+2.0; s.commitLatencyMs=base+1.0; s.holdLatencyMs=base+1.5; s.emergencyLatchLatencyMs=base+3.0; s.replayWriteLatencyMs=base+0.5; s.ackTimedOut=(step%97)==96; s.frameDropped=(step%89)==88; s.holdFallback=(step%53)==52; return s; } }
