#pragma once

#include "LockedInStatePreventer.h"
#include "NoPainGuarantee.h"
#include "NeuralDoseLimiter.h"
#include "AdaptiveSafetyEnvelope.h"
#include "../full_dive/FullDiveNeuralFrame.h"

#include <string>
#include <vector>

namespace dnxr {

enum class FullDiveSafetyAction { Allow, Reduce, HoldLastSafeFrame, RealityAnchor, Decompress, EmergencyStop };

struct FullDiveSafetyDecision {
    bool allowed = false;
    FullDiveSafetyAction action = FullDiveSafetyAction::EmergencyStop;
    std::string reason = "not_evaluated";
    std::string severity = "critical";
    std::vector<FullDiveModality> modalitiesToSuppress;
    float maxAllowedIntensityScale = 0.0f;
    bool requiresRealityAnchor = true;
    bool requiresDecompression = false;
    bool requiresEmergencyStop = false;
    bool requiresPhysicianCheck = false;
    bool requiresRecalibration = false;
    std::string auditCode = "FDSS_UNEVALUATED";
};

class FullDiveSafetySupervisor {
public:
    FullDiveSafetyDecision evaluate(const FullDiveNeuralFrame& frame) const;
};

} // namespace dnxr
