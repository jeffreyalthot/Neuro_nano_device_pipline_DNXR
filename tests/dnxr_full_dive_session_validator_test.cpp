#include "../src/full_dive/FullDiveSessionValidator.h"
#include <cassert>
int main(){ dnxr::FullDiveSessionValidator v; dnxr::FullDiveSessionEvidence e; auto r=v.validate(e); assert(!r.valid); e.emergencyExitAvailable=e.realityAnchorAvailable=e.decompressionAvailable=e.calibrationStable=e.temporalCoherenceMaintained=true; e.sensoryExposureRatio=0.8; r=v.validate(e); assert(r.valid); return 0; }
