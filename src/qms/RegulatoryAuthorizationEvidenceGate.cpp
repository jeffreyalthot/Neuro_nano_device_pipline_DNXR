#include "qms/RegulatoryAuthorizationEvidenceGate.h"
#include <filesystem>
#include <utility>

namespace dnxr {
namespace fs = std::filesystem;
RegulatoryAuthorizationEvidenceGate::RegulatoryAuthorizationEvidenceGate(std::string root) : root_(std::move(root)) {}
RegulatoryAuthorizationEvidenceDecision RegulatoryAuthorizationEvidenceGate::evaluate() const {
    RegulatoryAuthorizationEvidenceDecision d;
    auto present = [&](const char* p){ return fs::exists(fs::path(root_) / p); };
    d.irbApprovalPresent = present("qms/signoff/IRB_APPROVAL.md");
    d.rebApprovalPresent = present("qms/signoff/REB_APPROVAL.md");
    d.fdaIdeAuthorizationPresent = present("qms/signoff/FDA_IDE_AUTHORIZATION.md");
    d.healthCanadaItaAuthorizationPresent = present("qms/signoff/HEALTH_CANADA_ITA_AUTHORIZATION.md");
    d.physicianInvestigatorSignoffPresent = present("qms/signoff/PHYSICIAN_INVESTIGATOR_SIGNOFF.md");
    d.hardwareBenchSignoffPresent = present("qms/signoff/HARDWARE_BENCH_SIGNOFF.md");
    if (!d.irbApprovalPresent) d.missingArtifacts.push_back("qms/signoff/IRB_APPROVAL.md");
    if (!d.rebApprovalPresent) d.missingArtifacts.push_back("qms/signoff/REB_APPROVAL.md");
    if (!d.fdaIdeAuthorizationPresent) d.missingArtifacts.push_back("qms/signoff/FDA_IDE_AUTHORIZATION.md");
    if (!d.healthCanadaItaAuthorizationPresent) d.missingArtifacts.push_back("qms/signoff/HEALTH_CANADA_ITA_AUTHORIZATION.md");
    if (!d.physicianInvestigatorSignoffPresent) d.missingArtifacts.push_back("qms/signoff/PHYSICIAN_INVESTIGATOR_SIGNOFF.md");
    if (!d.hardwareBenchSignoffPresent) d.missingArtifacts.push_back("qms/signoff/HARDWARE_BENCH_SIGNOFF.md");
    d.completeExternalPackage = d.missingArtifacts.empty();
    d.allowHumanUse = false;
    d.allowStimulusDispatch = false;
    return d;
}
} // namespace dnxr
