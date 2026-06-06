#include "../src/readiness/RegulatoryClaimGuard.h"

#include <cassert>
#include <string>

static dnxr::EvidenceArtifact signedArtifact(const std::string& type) {
    return {"EV-" + type, "qms/generated/" + type + ".md", type, "REQ-" + type, "H-CLAIM",
            dnxr::EvidenceReviewState::Signed, "independent reviewer", "2026-06-05"};
}

int main() {
    dnxr::RegulatoryClaimGuard guard;
    dnxr::RegulatoryClaimEvidenceContext simulation;
    simulation.fullDiveEvidence = dnxr::FullDiveEvidenceLevel::IntegratedSimulation;

    auto simulationClaim = guard.evaluate(dnxr::RegulatoryClaim::SimulationOnlyPlatform, simulation);
    assert(simulationClaim.allowed);

    auto preclinicalPackage = guard.evaluate(dnxr::RegulatoryClaim::PreclinicalSoftwareReadinessPackage, simulation);
    assert(preclinicalPackage.allowed);

    auto humanFullDive = guard.evaluate(dnxr::RegulatoryClaim::HumanFullDiveImmersion, simulation);
    assert(!humanFullDive.allowed);
    assert(humanFullDive.blockers.size() >= 3);

    dnxr::EvidenceRepository repo;
    for (const std::string type : {"physical_hil_session", "preclinical_safety_report", "risk_control_verification", "runtime_10k_full",
                                  "clinical_protocol_package", "REB_IRB_approval", "Health_Canada_ITA_or_IDE_authorization",
                                  "independent_medical_signoff", "external_human_use_authorization"}) {
        repo.add(signedArtifact(type));
    }

    dnxr::RegulatoryClaimEvidenceContext clinical;
    clinical.fullDiveEvidence = dnxr::FullDiveEvidenceLevel::ClinicalEvidenceAvailable;
    clinical.clinicalReadiness = dnxr::ClinicalReadinessLevel::ClinicalTestReady;
    auto clinicalReady = guard.evaluate(dnxr::RegulatoryClaim::ClinicalTestReady, clinical, repo);
    assert(clinicalReady.allowed);

    auto unsafeHumanUseClaim = guard.evaluate(dnxr::RegulatoryClaim::SafeForHumanUse, clinical, repo);
    assert(!unsafeHumanUseClaim.allowed);

    repo.add(signedArtifact("clinical_evidence"));
    repo.add(signedArtifact("medical_approval"));
    auto approvedHumanUseClaim = guard.evaluate(dnxr::RegulatoryClaim::SafeForHumanUse, clinical, repo);
    assert(approvedHumanUseClaim.allowed);

    return 0;
}
