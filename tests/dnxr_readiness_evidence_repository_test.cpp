#include "../src/readiness/EvidenceRepository.h"
#include <cassert>

int main(){
    dnxr::EvidenceRepository repo;
    dnxr::EvidenceArtifact a{"EV-001","qms/hil/session_reports/HIL-SESSION-0.4.3.md","physical_hil_session","REQ-HIL","H-001",dnxr::EvidenceReviewState::Signed,"independent reviewer","2026-06-04"};
    repo.add(a);
    assert(repo.hasSignedEvidenceForHazard("H-001"));
    assert(repo.hasSignedEvidenceForRequirement("REQ-HIL"));
    assert(repo.hasSignedEvidenceOfType("physical_hil_session"));
    assert(repo.hasReviewedOrSignedEvidenceOfType("physical_hil_session"));
    assert(repo.forType("physical_hil_session").size()==1);
    return 0;
}
