#include "../src/readiness/ClinicalTestReadinessAssessor.h"
#include <cassert>
#include <string>

static dnxr::ReadinessScorecard fullScorecard(){ dnxr::ReadinessScorecard s; s.fullDiveCapabilityEvidence=s.integratedSimulationEvidence=s.hilEvidenceComplete=s.riskManagementComplete=s.traceabilityComplete=s.cybersecurityDocsComplete=s.verificationValidationComplete=s.qmsDocsComplete=s.physicianReviewPackageComplete=s.canadaUsReadinessDocsComplete=s.protocolReadinessPackageComplete=true; return s; }
static dnxr::EvidenceArtifact signedArtifact(const std::string& type){ return {"EV-"+type,"qms/generated/"+type+".md",type,"REQ-"+type,"H-001",dnxr::EvidenceReviewState::Signed,"named independent reviewer","2026-06-04"}; }
int main(){
    dnxr::ClinicalTestReadinessAssessor assessor;
    auto s=fullScorecard();
    assert(!assessor.canReturnClinicalTestReady(s));
    auto scorecardOnly=assessor.assess(s);
    assert(!scorecardOnly.clinicalTestReady);
    dnxr::EvidenceRepository repo;
    repo.add({"EV-DRAFT","qms/generated/draft.md","physical_hil_session","REQ-HIL","H-001",dnxr::EvidenceReviewState::Generated,"generator","2026-06-04"});
    assert(!assessor.assess(s,repo).clinicalTestReady);
    for(const std::string type: {"physical_hil_session","hil_latency_report","risk_control_verification","runtime_10k_full","cybersecurity_executed","canada_regulatory_package","us_regulatory_package","clinical_protocol_package","independent_signoff"}) repo.add(signedArtifact(type));
    auto ready=assessor.assess(s,repo);
    assert(ready.clinicalTestReady);
    assert(ready.level==dnxr::ClinicalReadinessLevel::ClinicalTestReady);
    return 0;
}
