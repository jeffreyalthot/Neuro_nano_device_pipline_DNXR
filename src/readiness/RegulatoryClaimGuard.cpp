#include "readiness/RegulatoryClaimGuard.h"

namespace dnxr {

const char* toString(RegulatoryClaim claim) {
    switch (claim) {
    case RegulatoryClaim::SimulationOnlyPlatform: return "SimulationOnlyPlatform";
    case RegulatoryClaim::PreclinicalSoftwareReadinessPackage: return "PreclinicalSoftwareReadinessPackage";
    case RegulatoryClaim::HumanFullDiveImmersion: return "HumanFullDiveImmersion";
    case RegulatoryClaim::ClinicalTestReady: return "ClinicalTestReady";
    case RegulatoryClaim::SafeForHumanUse: return "SafeForHumanUse";
    case RegulatoryClaim::TherapeuticUse: return "TherapeuticUse";
    case RegulatoryClaim::ImplantReady: return "ImplantReady";
    case RegulatoryClaim::MedicalApproved: return "MedicalApproved";
    }
    return "Unknown";
}

namespace {

bool atLeast(FullDiveEvidenceLevel actual, FullDiveEvidenceLevel minimum) {
    return static_cast<int>(actual) >= static_cast<int>(minimum);
}

bool atLeast(ClinicalReadinessLevel actual, ClinicalReadinessLevel minimum) {
    return static_cast<int>(actual) >= static_cast<int>(minimum);
}

void require(bool condition, const std::string& blocker, std::vector<std::string>& blockers) {
    if (!condition) {
        blockers.push_back(blocker);
    }
}

bool hasSignedEvidenceBundle(const EvidenceRepository& evidence, const std::vector<const char*>& types) {
    for (const char* type : types) {
        if (!evidence.hasSignedEvidenceOfType(type)) {
            return false;
        }
    }
    return true;
}

std::vector<const char*> preclinicalEvidenceTypes() {
    return {"physical_hil_session", "hil_latency_report", "risk_control_verification", "runtime_10k_full"};
}

std::vector<const char*> clinicalAuthorizationEvidenceTypes() {
    return {"clinical_protocol_package", "canada_regulatory_package", "us_regulatory_package", "independent_signoff"};
}

} // namespace

RegulatoryClaimDecision RegulatoryClaimGuard::evaluate(RegulatoryClaim claim, const RegulatoryClaimEvidenceContext& context) const {
    RegulatoryClaimDecision decision;

    switch (claim) {
    case RegulatoryClaim::SimulationOnlyPlatform:
        decision.allowed = true;
        decision.requiredEvidence = "Allowed as a simulation-only, not-for-human-use claim.";
        return decision;
    case RegulatoryClaim::PreclinicalSoftwareReadinessPackage:
        decision.requiredEvidence = "Integrated simulation or better with explicit non-clinical labeling.";
        require(atLeast(context.fullDiveEvidence, FullDiveEvidenceLevel::IntegratedSimulation),
                "full-dive capability evidence below IntegratedSimulation", decision.blockers);
        break;
    case RegulatoryClaim::HumanFullDiveImmersion:
        decision.requiredEvidence =
            "Human full-dive immersion claim requires qualified physical HIL, complete preclinical evidence, and clinical evidence.";
        require(context.physicalHilQualified, "qualified physical HIL evidence is missing", decision.blockers);
        require(context.preclinicalEvidenceComplete, "preclinical evidence is incomplete", decision.blockers);
        require(context.clinicalEvidenceAvailable, "human clinical evidence is unavailable", decision.blockers);
        require(atLeast(context.fullDiveEvidence, FullDiveEvidenceLevel::ClinicalEvidenceAvailable),
                "full-dive capability matrix is below ClinicalEvidenceAvailable", decision.blockers);
        break;
    case RegulatoryClaim::ClinicalTestReady:
        decision.requiredEvidence =
            "Clinical-test-ready claim requires ClinicalTestReady readiness and authorized investigational use.";
        require(atLeast(context.clinicalReadiness, ClinicalReadinessLevel::ClinicalTestReady),
                "clinical readiness is below ClinicalTestReady", decision.blockers);
        require(context.authorizedInvestigationalUse, "applicable investigational authorization is missing", decision.blockers);
        break;
    case RegulatoryClaim::SafeForHumanUse:
        decision.requiredEvidence =
            "Safe-for-human-use claim requires clinical evidence, applicable authorization, and medical approval.";
        require(context.clinicalEvidenceAvailable, "human safety/clinical evidence is unavailable", decision.blockers);
        require(context.authorizedInvestigationalUse, "applicable human-use authorization is missing", decision.blockers);
        require(context.medicalApprovalGranted, "medical approval/clearance is not recorded", decision.blockers);
        break;
    case RegulatoryClaim::TherapeuticUse:
        decision.requiredEvidence =
            "Therapeutic claim requires clinical evidence for the intended use and medical approval/clearance.";
        require(context.clinicalEvidenceAvailable, "therapeutic clinical evidence is unavailable", decision.blockers);
        require(context.medicalApprovalGranted, "therapeutic medical approval/clearance is not recorded", decision.blockers);
        break;
    case RegulatoryClaim::ImplantReady:
        decision.requiredEvidence =
            "Implant-ready claim requires preclinical evidence, clinical evidence, and medical approval/clearance.";
        require(context.preclinicalEvidenceComplete, "implant preclinical evidence is incomplete", decision.blockers);
        require(context.clinicalEvidenceAvailable, "implant clinical evidence is unavailable", decision.blockers);
        require(context.medicalApprovalGranted, "implant approval/clearance is not recorded", decision.blockers);
        break;
    case RegulatoryClaim::MedicalApproved:
        decision.requiredEvidence = "Medical-approved claim requires recorded approval/clearance for the stated intended use.";
        require(context.medicalApprovalGranted, "medical approval/clearance is not recorded", decision.blockers);
        break;
    }

    decision.allowed = decision.blockers.empty();
    return decision;
}

RegulatoryClaimDecision RegulatoryClaimGuard::evaluate(RegulatoryClaim claim, const RegulatoryClaimEvidenceContext& context,
                                                       const EvidenceRepository& evidence) const {
    RegulatoryClaimEvidenceContext enriched = context;
    if (hasSignedEvidenceBundle(evidence, preclinicalEvidenceTypes())) {
        enriched.preclinicalEvidenceComplete = true;
        enriched.physicalHilQualified = true;
    }
    if (evidence.hasSignedEvidenceOfType("clinical_evidence")) {
        enriched.clinicalEvidenceAvailable = true;
    }
    if (hasSignedEvidenceBundle(evidence, clinicalAuthorizationEvidenceTypes())) {
        enriched.authorizedInvestigationalUse = true;
    }
    if (evidence.hasSignedEvidenceOfType("medical_approval")) {
        enriched.medicalApprovalGranted = true;
    }
    return evaluate(claim, enriched);
}

RegulatoryClaimEvidenceContext RegulatoryClaimGuard::contextFromCapabilityMatrix(const FullDiveCapabilityMatrix& matrix,
                                                                                 ClinicalReadinessLevel readiness) {
    RegulatoryClaimEvidenceContext context;
    context.clinicalReadiness = readiness;
    context.fullDiveEvidence = FullDiveEvidenceLevel::ClinicalEvidenceAvailable;
    for (const auto& evidence : matrix.allEvidence()) {
        if (static_cast<int>(evidence.level) < static_cast<int>(context.fullDiveEvidence)) {
            context.fullDiveEvidence = evidence.level;
        }
    }
    return context;
}

} // namespace dnxr
