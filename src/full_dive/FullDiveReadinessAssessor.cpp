#include "FullDiveReadinessAssessor.h"
namespace dnxr {
const char* toString(FullDiveReadinessStage stage) {
    switch(stage) {
    case FullDiveReadinessStage::BenchOnly: return "BenchOnly";
    case FullDiveReadinessStage::AnimalOrPhantomPreclinical: return "AnimalOrPhantomPreclinical";
    case FullDiveReadinessStage::FirstInHumanRestricted: return "FirstInHumanRestricted";
    case FullDiveReadinessStage::HumanClinicalExpanded: return "HumanClinicalExpanded";
    case FullDiveReadinessStage::NotApprovedForCommercialUse: return "NotApprovedForCommercialUse";
    }
    return "Unknown";
}
FullDiveReadinessReport FullDiveReadinessAssessor::assess(const FullDiveCapabilityMatrix& matrix) const {
    FullDiveReadinessReport r;
    r.gaps = matrix.capabilitiesBelow(FullDiveEvidenceLevel::IntegratedSimulation);
    r.integratedSimulationReady = r.gaps.empty();
    r.hilReady = matrix.allAtLeast(FullDiveEvidenceLevel::HILValidated);
    if (!r.hilReady) {
        auto hilGaps = matrix.capabilitiesBelow(FullDiveEvidenceLevel::HILValidated);
        r.gaps.insert(r.gaps.end(), hilGaps.begin(), hilGaps.end());
    }
    if (matrix.allAtLeast(FullDiveEvidenceLevel::ClinicalEvidenceAvailable)) {
        r.stage = FullDiveReadinessStage::HumanClinicalExpanded;
    } else if (matrix.allAtLeast(FullDiveEvidenceLevel::PreclinicalEvidenceAvailable)) {
        r.stage = FullDiveReadinessStage::FirstInHumanRestricted;
    } else if (r.hilReady) {
        r.stage = FullDiveReadinessStage::AnimalOrPhantomPreclinical;
    } else {
        r.stage = FullDiveReadinessStage::BenchOnly;
    }
    return r;
}
}
