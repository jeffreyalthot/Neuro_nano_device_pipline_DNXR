#include "ClinicalTestReadinessAssessor.h"

#include <array>

namespace dnxr {

const char* toString(ClinicalReadinessLevel level) {
    switch (level) {
    case ClinicalReadinessLevel::SimulationOnly: return "SimulationOnly";
    case ClinicalReadinessLevel::IntegratedSimulation: return "IntegratedSimulation";
    case ClinicalReadinessLevel::HILReady: return "HILReady";
    case ClinicalReadinessLevel::PreclinicalEvidenceComplete: return "PreclinicalEvidenceComplete";
    case ClinicalReadinessLevel::RegulatoryPackageDraftReady: return "RegulatoryPackageDraftReady";
    case ClinicalReadinessLevel::ClinicalSubmissionReady: return "ClinicalSubmissionReady";
    case ClinicalReadinessLevel::ClinicalTestAcceptanceReviewReady: return "ClinicalTestAcceptanceReviewReady";
    case ClinicalReadinessLevel::ClinicalTestReady: return "ClinicalTestReady";
    }
    return "Unknown";
}

namespace {

bool baseScorecardComplete(const ReadinessScorecard& s) {
    return s.fullDiveCapabilityEvidence && s.integratedSimulationEvidence && s.hilEvidenceComplete &&
           s.riskManagementComplete && s.traceabilityComplete && s.cybersecurityDocsComplete &&
           s.verificationValidationComplete && s.qmsDocsComplete && s.physicianReviewPackageComplete &&
           s.canadaUsReadinessDocsComplete && s.protocolReadinessPackageComplete;
}

bool hasPreclinicalSignedEvidence(const EvidenceRepository& evidence) {
    return evidence.hasSignedEvidenceOfType("physical_hil_session") &&
           evidence.hasSignedEvidenceOfType("hil_latency_report") &&
           evidence.hasSignedEvidenceOfType("risk_control_verification") &&
           evidence.hasSignedEvidenceOfType("runtime_10k_full");
}

bool hasSubmissionSignedEvidence(const EvidenceRepository& evidence) {
    return hasPreclinicalSignedEvidence(evidence) && evidence.hasSignedEvidenceOfType("cybersecurity_executed") &&
           evidence.hasSignedEvidenceOfType("canada_regulatory_package") &&
           evidence.hasSignedEvidenceOfType("us_regulatory_package") &&
           evidence.hasSignedEvidenceOfType("clinical_protocol_package") &&
           evidence.hasSignedEvidenceOfType("independent_signoff");
}

void addMissingEvidenceGaps(std::vector<std::string>& gaps, const EvidenceRepository& evidence) {
    constexpr std::array<const char*, 9> requiredTypes = {
        "physical_hil_session",
        "hil_latency_report",
        "risk_control_verification",
        "runtime_10k_full",
        "cybersecurity_executed",
        "canada_regulatory_package",
        "us_regulatory_package",
        "clinical_protocol_package",
        "independent_signoff",
    };
    for (const char* type : requiredTypes) {
        if (!evidence.hasSignedEvidenceOfType(type)) {
            gaps.push_back(std::string("signed evidence artifact required: ") + type);
        }
    }
}

} // namespace

bool ClinicalTestReadinessAssessor::canReturnClinicalTestReady(const ReadinessScorecard&) const {
    return false;
}

bool ClinicalTestReadinessAssessor::canReturnClinicalTestReady(const ReadinessScorecard& s, const EvidenceRepository& evidence) const {
    return baseScorecardComplete(s) && hasSubmissionSignedEvidence(evidence);
}

ClinicalReadinessAssessment ClinicalTestReadinessAssessor::assess(const ReadinessScorecard& s) const {
    ClinicalReadinessAssessment result;
    result.score = s.completionRatio();
    result.gaps = s.missingClinicalTestReadyItems();
    if (baseScorecardComplete(s)) {
        result.gaps.push_back("EvidenceRepository signed artifacts required; scorecard booleans cannot unlock ClinicalTestReady");
        result.level = ClinicalReadinessLevel::RegulatoryPackageDraftReady;
    } else if (s.hilEvidenceComplete && s.riskManagementComplete && s.traceabilityComplete &&
               s.verificationValidationComplete) {
        result.level = ClinicalReadinessLevel::PreclinicalEvidenceComplete;
    } else if (s.fullDiveCapabilityEvidence && s.integratedSimulationEvidence && s.hilEvidenceComplete) {
        result.level = ClinicalReadinessLevel::HILReady;
    } else if (s.fullDiveCapabilityEvidence && s.integratedSimulationEvidence) {
        result.level = ClinicalReadinessLevel::IntegratedSimulation;
    }
    result.clinicalTestReady = false;
    return result;
}

ClinicalReadinessAssessment ClinicalTestReadinessAssessor::assess(const ReadinessScorecard& s, const EvidenceRepository& evidence) const {
    ClinicalReadinessAssessment result = assess(s);
    result.gaps = s.missingClinicalTestReadyItems();
    addMissingEvidenceGaps(result.gaps, evidence);
    result.clinicalTestReady = canReturnClinicalTestReady(s, evidence);

    if (result.clinicalTestReady) {
        result.level = ClinicalReadinessLevel::ClinicalTestReady;
        result.gaps.clear();
    } else if (s.hilEvidenceComplete && s.riskManagementComplete && s.traceabilityComplete && s.cybersecurityDocsComplete &&
               s.verificationValidationComplete && s.qmsDocsComplete && s.physicianReviewPackageComplete &&
               s.canadaUsReadinessDocsComplete && s.protocolReadinessPackageComplete &&
               evidence.hasSignedEvidenceOfType("runtime_10k_full") && evidence.hasSignedEvidenceOfType("cybersecurity_executed") &&
               evidence.hasSignedEvidenceOfType("independent_signoff")) {
        result.level = ClinicalReadinessLevel::ClinicalTestAcceptanceReviewReady;
    } else if (s.hilEvidenceComplete && s.riskManagementComplete && s.traceabilityComplete && s.cybersecurityDocsComplete &&
               s.verificationValidationComplete && s.qmsDocsComplete && s.physicianReviewPackageComplete &&
               s.canadaUsReadinessDocsComplete && hasSubmissionSignedEvidence(evidence)) {
        result.level = ClinicalReadinessLevel::ClinicalSubmissionReady;
    } else if (s.hilEvidenceComplete && s.riskManagementComplete && s.traceabilityComplete && s.cybersecurityDocsComplete &&
               s.qmsDocsComplete) {
        result.level = ClinicalReadinessLevel::RegulatoryPackageDraftReady;
    } else if (s.hilEvidenceComplete && s.riskManagementComplete && s.traceabilityComplete && s.verificationValidationComplete &&
               hasPreclinicalSignedEvidence(evidence)) {
        result.level = ClinicalReadinessLevel::PreclinicalEvidenceComplete;
    }
    return result;
}

} // namespace dnxr
