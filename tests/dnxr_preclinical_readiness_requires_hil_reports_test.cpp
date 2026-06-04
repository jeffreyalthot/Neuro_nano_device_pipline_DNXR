#include "../src/readiness/PreclinicalReadinessAssessor.h"
#include <cassert>
#include <string>
static dnxr::EvidenceArtifact ev(const std::string& type){ return {"EV-"+type,"qms/"+type+".md",type,"REQ-"+type,"H-001",dnxr::EvidenceReviewState::Signed,"reviewer","2026-06-04"}; }
int main(){ dnxr::PreclinicalReadinessAssessor a; dnxr::PreclinicalEvidenceChecklist c; c.fullDiveSessionValidated=c.hilFaultInjectionPassed=c.latencyBudgetPassed=c.emergencyStopPassed=c.recoverySequencePassed=c.riskControlsVerified=true; dnxr::EvidenceRepository repo; assert(!a.assess(c,repo).ready); for(const std::string type:{"physical_hil_session","hil_latency_report","risk_control_verification","runtime_10k_full"}) repo.add(ev(type)); assert(a.assess(c,repo).ready); return 0; }
