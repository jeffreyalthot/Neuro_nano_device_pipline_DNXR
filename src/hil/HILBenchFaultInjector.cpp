#include "hil/HILBenchFaultInjector.h"
namespace dnxr { BenchDeviceSample HILBenchFaultInjector::applyTimeoutFault(BenchDeviceSample s) const { s.ackTimedOut=true; s.holdFallback=true; return s; } BenchDeviceSample HILBenchFaultInjector::applyDisconnectFault(BenchDeviceSample s) const { s.frameDropped=true; s.holdFallback=true; return s; } }
