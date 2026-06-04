#include "FullDiveSessionValidator.h"
namespace dnxr {
FullDiveSessionValidationResult FullDiveSessionValidator::validate(const FullDiveSessionEvidence& e) const { FullDiveSessionValidationResult r; if(!e.emergencyExitAvailable) r.failures.push_back("emergency exit missing"); if(!e.realityAnchorAvailable) r.failures.push_back("reality anchor missing"); if(!e.decompressionAvailable) r.failures.push_back("decompression missing"); if(!e.calibrationStable) r.failures.push_back("calibration unstable"); if(!e.temporalCoherenceMaintained) r.failures.push_back("temporal coherence lost"); if(e.sensoryExposureRatio>1.0) r.failures.push_back("sensory exposure exceeded"); r.valid=r.failures.empty(); return r; }
}
