#pragma once
#include <vector>
namespace dnxr { class StabilityMonitor { public: bool checkCardioRespiratoryWindow(const std::vector<float>& telemetry) const; }; }
