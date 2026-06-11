#pragma once
#include "hil/BenchDeviceAdapter.h"
namespace dnxr { class HILBenchFaultInjector { public: BenchDeviceSample applyTimeoutFault(BenchDeviceSample sample) const; BenchDeviceSample applyDisconnectFault(BenchDeviceSample sample) const; }; }
