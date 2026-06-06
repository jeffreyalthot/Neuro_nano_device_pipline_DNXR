#include "../src/clinical/ClinicalEvidenceVerifier.h"
#include "../src/clinical/ClinicalModeGate.h"
#include <cassert>
int main(){ std::vector<dnxr::ClinicalEvidenceRecord> records; for(auto kind: dnxr::ClinicalEvidenceVerifier::requiredHumanUseEvidence()){ dnxr::ClinicalEvidenceRecord r; r.kind=kind; r.artifactId="artifact"; r.hash="1234567890123456"; r.signature="unsigned"; r.author="qa"; r.role="reviewer"; r.approved=true; r.signedAtUnixSeconds=1; r.expiresAtUnixSeconds=2000; records.push_back(r); } dnxr::ClinicalModeGate gate; auto d=gate.evaluateHumanUseRequest(records,1000); assert(!d.humanUseAllowed); return 0; }
