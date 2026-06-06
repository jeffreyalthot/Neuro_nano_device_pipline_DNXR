#include "VestibularConflictGuard.h"
#include <algorithm>
#include <cmath>
namespace dnxr {
VestibularConflictDecision VestibularConflictGuard::evaluate(const VestibularTelemetryFrame& f) const { VestibularConflictDecision d; if(!std::isfinite(f.visualAngularVelocity)||!std::isfinite(f.measuredHeadAngularVelocity)||!std::isfinite(f.latencyMs)){ d.blockers.push_back("invalid_vestibular_frame"); } double visualHead=std::abs(f.visualAngularVelocity-f.measuredHeadAngularVelocity); double proprio=std::abs(f.visualAngularVelocity-f.proprioceptiveVelocity); double haptic=std::abs(f.visualAngularVelocity-f.hapticMotionCue); d.conflictScore=std::min(1.0,(visualHead+0.5*proprio+0.5*haptic)/180.0 + f.latencyMs/250.0); if(d.conflictScore>0.35) d.blockers.push_back("vestibular_conflict_limit_exceeded"); if(f.latencyMs>50.0) d.blockers.push_back("vestibular_latency_limit_exceeded"); d.allowed=d.blockers.empty(); return d; }
}
