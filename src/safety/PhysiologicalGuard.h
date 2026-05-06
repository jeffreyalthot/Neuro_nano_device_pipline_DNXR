#pragma once
#include "StabilityMonitor.h"
#include "EmergencyShutdown.h"
#include <vector>
namespace dnxr { class PhysiologicalGuard { public: bool enforce(const std::vector<float>& telemetry) const; private: StabilityMonitor monitor_; EmergencyShutdown killSwitch_; }; }
