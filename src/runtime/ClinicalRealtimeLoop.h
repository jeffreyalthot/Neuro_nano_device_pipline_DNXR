#pragma once
#include "DeadlineViolationPolicy.h"
#include <string>
#include <vector>
namespace dnxr {
struct ClinicalRealtimeBudgets { double hardSafetyLoopMs=2.0; double telemetryLoopMs=5.0; double evidenceLoopMs=10.0; double softExperienceLoopMs=20.0; };
struct ClinicalRealtimeDecision { bool emergencyLatched=false; std::vector<std::string> events; };
class ClinicalRealtimeLoop { public: explicit ClinicalRealtimeLoop(ClinicalRealtimeBudgets budgets = {}); ClinicalRealtimeDecision runSafetyCycle(const std::vector<DeadlineObservation>& observations) const; private: ClinicalRealtimeBudgets budgets_; };
}
