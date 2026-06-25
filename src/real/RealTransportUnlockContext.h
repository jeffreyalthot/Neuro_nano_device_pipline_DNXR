#pragma once
#include "RealReadinessGate.h"
namespace dnxr { struct RealTransportUnlockContext { RealReadinessEvidence evidence; RealReadinessDecision decision; bool valid() const { return decision.allowed; } }; }
