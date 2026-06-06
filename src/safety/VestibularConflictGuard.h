#pragma once
#include <string>
#include <vector>
namespace dnxr {
struct VestibularTelemetryFrame { double visualAngularVelocity=0.0; double measuredHeadAngularVelocity=0.0; double proprioceptiveVelocity=0.0; double hapticMotionCue=0.0; double latencyMs=0.0; };
struct VestibularConflictDecision { bool allowed=false; double conflictScore=1.0; std::vector<std::string> blockers; };
class VestibularConflictGuard { public: VestibularConflictDecision evaluate(const VestibularTelemetryFrame& frame) const; };
}
