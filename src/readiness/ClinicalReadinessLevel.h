#pragma once

namespace dnxr {

enum class ClinicalReadinessLevel {
    SimulationOnly,
    IntegratedSimulation,
    HILReady,
    PreclinicalEvidenceComplete,
    RegulatoryPackageDraftReady,
    ClinicalSubmissionReady,
    ClinicalTestAcceptanceReviewReady,
    ClinicalTestReady
};

const char* toString(ClinicalReadinessLevel level);

} // namespace dnxr
