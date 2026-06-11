#include "../src/full_dive/FullDiveCapabilityMatrix.h"
#include <cassert>
int main(){ dnxr::FullDiveCapabilityMatrix m; assert(m.requiredCapabilities().size()>=13); assert(!m.allAtLeast(dnxr::FullDiveEvidenceLevel::SimulatedOnly)); m.setEvidence("CAP-VISUAL", dnxr::FullDiveEvidenceLevel::HILValidated, "qms/generated/visual.md", true); assert(m.evidenceFor("CAP-VISUAL").safetyControlLinked); return 0; }
