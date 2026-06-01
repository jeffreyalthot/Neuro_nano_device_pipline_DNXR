#pragma once

#include "SafetyGovernor.h"

#include <string>
#include <vector>

namespace dnxr {

struct SafetyLayerDecision {
    std::string layerName;
    SafetyAction action = SafetyAction::Continue;
    float riskScore = 0.0f;
    float intensityScale = 1.0f;
    bool allowRender = true;
    bool requiresEmergencyStop = false;
    std::string reason = "nominal";
};

struct SafetyDecisionTrace {
    std::vector<SafetyLayerDecision> layerDecisions;
    SafetyDecision finalDecision;

    bool auditedEveryLayer() const;
    std::string summarize() const;
};

} // namespace dnxr
