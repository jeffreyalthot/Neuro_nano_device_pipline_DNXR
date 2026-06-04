#include "../src/full_dive/FullDiveReadinessAssessor.h"
#include <cassert>
int main(){ dnxr::FullDiveCapabilityMatrix m; for(const auto& id: dnxr::FullDiveCapabilityMatrix::requiredCapabilities()) m.setEvidence(id,dnxr::FullDiveEvidenceLevel::IntegratedSimulation,"artifact",true); dnxr::FullDiveReadinessAssessor a; auto r=a.assess(m); assert(r.integratedSimulationReady); assert(!r.hilReady); return 0; }
