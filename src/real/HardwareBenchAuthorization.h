#pragma once
#include "RealReadinessGate.h"
namespace dnxr { class HardwareBenchAuthorization { public: RealReadinessDecision authorize(const RealReadinessEvidence& evidence) const { return RealReadinessGate{}.evaluate(evidence); } }; }
