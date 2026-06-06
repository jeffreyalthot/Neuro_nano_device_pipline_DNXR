#include "AdaptiveSafetyEnvelope.h"
#include <algorithm>
namespace dnxr { float AdaptiveSafetyEnvelope::intensityScale(const AdaptiveSafetyInputs& i) const { float s=1.0f-0.25f*i.stress-0.20f*i.fatigue-0.30f*(1.0f-i.signalQuality)-0.25f*i.calibrationDrift-0.01f*i.latency; if(i.patientStopUncertain) s*=0.25f; return std::clamp(s,0.0f,1.0f); } }
