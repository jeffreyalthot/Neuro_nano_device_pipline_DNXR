#pragma once
#include "RuntimeLatencyProfiler.h"
#include <cstddef>
namespace dnxr { struct RuntimeDeterminismReport { RuntimeLatencyReport latency; double jitterMs=0.0; std::size_t allocationGrowth=0; int droppedFrames=0; int emergencyEvents=0; bool deterministicEnough(double latencyBudgetMs, double jitterBudgetMs) const { return latency.worst <= latencyBudgetMs && jitterMs <= jitterBudgetMs && allocationGrowth == 0; } }; }
