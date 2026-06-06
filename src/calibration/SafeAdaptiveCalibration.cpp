#include "SafeAdaptiveCalibration.h"
#include <algorithm>
namespace dnxr { float SafeAdaptiveCalibration::boundedDelta(const AdaptiveCalibrationInput& i,double minutes) const { if(i.emergency||i.poorSignal||i.patientDistress) return 0.0f; return std::clamp(i.requestedDelta,-0.02f*static_cast<float>(minutes),0.02f*static_cast<float>(minutes)); } }
