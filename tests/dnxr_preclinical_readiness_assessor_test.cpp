#include "../src/readiness/PreclinicalReadinessAssessor.h"
#include <cassert>
int main(){ dnxr::PreclinicalReadinessAssessor a; dnxr::PreclinicalEvidenceChecklist c; auto r=a.assess(c); assert(!r.ready); c.fullDiveSessionValidated=c.hilFaultInjectionPassed=c.latencyBudgetPassed=c.emergencyStopPassed=c.recoverySequencePassed=c.riskControlsVerified=true; r=a.assess(c); assert(r.ready); return 0; }
