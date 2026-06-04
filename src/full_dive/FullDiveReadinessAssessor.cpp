#include "FullDiveReadinessAssessor.h"
namespace dnxr {
FullDiveReadinessReport FullDiveReadinessAssessor::assess(const FullDiveCapabilityMatrix& matrix) const {
    FullDiveReadinessReport r;
    r.gaps = matrix.capabilitiesBelow(FullDiveEvidenceLevel::IntegratedSimulation);
    r.integratedSimulationReady = r.gaps.empty();
    r.hilReady = matrix.allAtLeast(FullDiveEvidenceLevel::HILValidated);
    if (!r.hilReady) {
        auto hilGaps = matrix.capabilitiesBelow(FullDiveEvidenceLevel::HILValidated);
        r.gaps.insert(r.gaps.end(), hilGaps.begin(), hilGaps.end());
    }
    return r;
}
}
