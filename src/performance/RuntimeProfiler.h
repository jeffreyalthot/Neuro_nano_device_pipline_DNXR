#pragma once
#include "LatencyTracker.h"
#include <string>
namespace dnxr { class RuntimeProfiler { public: void beginCycle() noexcept; double endCycle() noexcept; std::string report() const; private: LatencyTracker tracker_; }; }
