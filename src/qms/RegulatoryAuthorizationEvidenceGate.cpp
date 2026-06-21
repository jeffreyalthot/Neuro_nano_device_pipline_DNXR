#include "qms/RegulatoryAuthorizationEvidenceGate.h"
#include "qms/QmsSignatureVerifier.h"
#include <utility>

namespace dnxr {
RegulatoryAuthorizationEvidenceGate::RegulatoryAuthorizationEvidenceGate(std::string root) : root_(std::move(root)) {}
RegulatoryAuthorizationEvidenceDecision RegulatoryAuthorizationEvidenceGate::evaluate() const {
    RegulatoryAuthorizationEvidenceDecision d;
    QmsSignatureVerifier verifier(root_);
    auto accepted = [&](const char* p){ auto r = verifier.verifySignedArtifact(p); if (!r.present) d.missingArtifacts.push_back(p); else if (!r.accepted) d.invalidArtifacts.push_back(p); return r.accepted; };
    d.rebApprovalPresent = accepted("qms/signoff/REB_APPROVAL.signed.json");
    d.irbApprovalPresent = accepted("qms/signoff/IRB_APPROVAL.signed.json");
    d.healthCanadaItaAuthorizationPresent = accepted("qms/signoff/HEALTH_CANADA_ITA_AUTHORIZATION.signed.json");
    d.fdaIdeAuthorizationPresent = accepted("qms/signoff/FDA_IDE_AUTHORIZATION.signed.json");
    d.physicianInvestigatorSignoffPresent = accepted("qms/signoff/PHYSICIAN_INVESTIGATOR_SIGNOFF.signed.json");
    d.hardwareBenchSignoffPresent = accepted("qms/signoff/HARDWARE_BENCH_SIGNOFF.signed.json");
    d.clinicalSiteAuthorizationPresent = accepted("qms/signoff/CLINICAL_SITE_AUTHORIZATION.signed.json");
    d.qmsReleaseSignoffPresent = accepted("qms/signoff/QMS_RELEASE_SIGNOFF.signed.json");
    d.clinicalProtocolApprovalPresent = accepted("qms/signoff/CLINICAL_PROTOCOL_APPROVAL.signed.json");
    d.canadaPathValid = d.rebApprovalPresent && d.healthCanadaItaAuthorizationPresent;
    d.usaPathValid = d.irbApprovalPresent && d.fdaIdeAuthorizationPresent;
    d.regulatoryPathValid = d.canadaPathValid || d.usaPathValid;
    d.completeExternalPackage = d.regulatoryPathValid && d.physicianInvestigatorSignoffPresent && d.hardwareBenchSignoffPresent && d.clinicalSiteAuthorizationPresent && d.qmsReleaseSignoffPresent && d.clinicalProtocolApprovalPresent && d.invalidArtifacts.empty();
    d.allowHumanUse = d.completeExternalPackage;
    d.allowStimulusDispatch = false;
    return d;
}
} // namespace dnxr
