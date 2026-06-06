#include "PsychologicalSafetyMonitor.h"
#include <cmath>
namespace dnxr {
PsychologicalSafetyDecision PsychologicalSafetyMonitor::evaluate(const PsychologicalTelemetryFrame& f) const { PsychologicalSafetyDecision d; if(!std::isfinite(f.panicScore)||!std::isfinite(f.dissociationScore)||!std::isfinite(f.cognitiveLoad)) d.blockers.push_back("invalid_psychological_telemetry"); if(f.panicScore>0.35) d.blockers.push_back("panic_risk"); if(f.dissociationScore>0.25) d.blockers.push_back("dissociation_risk"); if(f.cognitiveLoad>0.50) d.blockers.push_back("clinical_cognitive_load_limit"); if(f.refusesExit) d.blockers.push_back("refusal_or_inability_to_exit"); if(f.compulsiveUseSignal) d.blockers.push_back("compulsive_use_signal"); if(f.identityConfusion) d.blockers.push_back("identity_confusion"); d.safe=d.blockers.empty(); return d; }
}
