#include "PatientSpecificSafetyEnvelope.h"
#include <algorithm>
namespace dnxr { float PatientSpecificSafetyEnvelope::riskMultiplier() const { return std::clamp(1.0f+dissociationRisk+cybersicknessRisk+seizureRisk+panicRisk-calibrationConfidence,0.25f,4.0f); } }
