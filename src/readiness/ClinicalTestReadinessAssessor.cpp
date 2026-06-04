#include "ClinicalTestReadinessAssessor.h"

namespace dnxr {

const char* toString(ClinicalReadinessLevel level) {
    switch (level) {
    case ClinicalReadinessLevel::SimulationOnly: return "SimulationOnly";
    case ClinicalReadinessLevel::IntegratedSimulation: return "IntegratedSimulation";
    case ClinicalReadinessLevel::HILReady: return "HILReady";
    case ClinicalReadinessLevel::PreclinicalEvidenceComplete: return "PreclinicalEvidenceComplete";
    case ClinicalReadinessLevel::RegulatoryPackageDraftReady: return "RegulatoryPackageDraftReady";
    case ClinicalReadinessLevel::ClinicalSubmissionReady: return "ClinicalSubmissionReady";
    case ClinicalReadinessLevel::ClinicalTestReady: return "ClinicalTestReady";
    }
    return "Unknown";
}

bool ClinicalTestReadinessAssessor::canReturnClinicalTestReady(const ReadinessScorecard& s) const {
    return s.fullDiveCapabilityEvidence && s.integratedSimulationEvidence && s.hilEvidenceComplete &&
           s.riskManagementComplete && s.traceabilityComplete && s.cybersecurityDocsComplete &&
           s.verificationValidationComplete && s.qmsDocsComplete && s.physicianReviewPackageComplete &&
           s.canadaUsReadinessDocsComplete && s.protocolReadinessPackageComplete;
}

ClinicalReadinessAssessment ClinicalTestReadinessAssessor::assess(const ReadinessScorecard& s) const {
    ClinicalReadinessAssessment result;
    result.score = s.completionRatio();
    result.gaps = s.missingClinicalTestReadyItems();
    result.clinicalTestReady = canReturnClinicalTestReady(s);

    if (result.clinicalTestReady) {
        result.level = ClinicalReadinessLevel::ClinicalTestReady;
    } else if (s.hilEvidenceComplete && s.riskManagementComplete && s.traceabilityComplete &&
               s.cybersecurityDocsComplete && s.verificationValidationComplete && s.qmsDocsComplete &&
               s.physicianReviewPackageComplete && s.canadaUsReadinessDocsComplete) {
        result.level = ClinicalReadinessLevel::ClinicalSubmissionReady;
    } else if (s.hilEvidenceComplete && s.riskManagementComplete && s.traceabilityComplete &&
               s.cybersecurityDocsComplete && s.qmsDocsComplete) {
        result.level = ClinicalReadinessLevel::RegulatoryPackageDraftReady;
    } else if (s.hilEvidenceComplete && s.riskManagementComplete && s.traceabilityComplete &&
               s.verificationValidationComplete) {
        result.level = ClinicalReadinessLevel::PreclinicalEvidenceComplete;
    } else if (s.fullDiveCapabilityEvidence && s.integratedSimulationEvidence && s.hilEvidenceComplete) {
        result.level = ClinicalReadinessLevel::HILReady;
    } else if (s.fullDiveCapabilityEvidence && s.integratedSimulationEvidence) {
        result.level = ClinicalReadinessLevel::IntegratedSimulation;
    }
    return result;
}

} // namespace dnxr
