#include "StabilityMonitor.h"
namespace dnxr { bool StabilityMonitor::checkCardioRespiratoryWindow(const std::vector<float>& telemetry) const { if(telemetry.size()<2) return false; float bpm=telemetry[0], rpm=telemetry[1]; return bpm>45.0F && bpm<130.0F && rpm>8.0F && rpm<28.0F; } }
