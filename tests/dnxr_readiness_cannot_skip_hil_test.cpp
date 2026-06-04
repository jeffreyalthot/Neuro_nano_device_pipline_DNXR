#include "../src/readiness/ClinicalTestReadinessAssessor.h"
#include <cassert>
int main(){ dnxr::ReadinessScorecard s; s.fullDiveCapabilityEvidence=s.integratedSimulationEvidence=s.riskManagementComplete=s.traceabilityComplete=s.cybersecurityDocsComplete=s.verificationValidationComplete=s.qmsDocsComplete=s.physicianReviewPackageComplete=s.canadaUsReadinessDocsComplete=s.protocolReadinessPackageComplete=true; dnxr::ClinicalTestReadinessAssessor a; auto r=a.assess(s); assert(!r.clinicalTestReady); assert(r.level!=dnxr::ClinicalReadinessLevel::ClinicalTestReady); return 0; }
