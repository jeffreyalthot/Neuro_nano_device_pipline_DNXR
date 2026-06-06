#pragma once
#include <string>
#include <vector>
namespace dnxr {
struct PsychologicalTelemetryFrame { double panicScore=0.0; double dissociationScore=0.0; double cognitiveLoad=0.0; bool refusesExit=false; bool compulsiveUseSignal=false; bool identityConfusion=false; };
struct PsychologicalSafetyDecision { bool safe=false; std::vector<std::string> blockers; };
class PsychologicalSafetyMonitor { public: PsychologicalSafetyDecision evaluate(const PsychologicalTelemetryFrame& frame) const; };
}
