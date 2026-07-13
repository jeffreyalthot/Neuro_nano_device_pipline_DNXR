#pragma once

#include "SafetyGovernor.h"

#include <cstdint>
#include <string>
#include <vector>

namespace dnxr {

enum class SafetyDecisionLevel {
    LocalEncoder,
    Modality,
    CrossModal,
    Session,
    TransportHardware,
    PostFeedback
};

struct SafetyLayerDecision {
    std::string layerName;
    SafetyDecisionLevel level = SafetyDecisionLevel::Session;
    SafetyAction action = SafetyAction::Continue;
    float riskScore = 0.0f;
    float intensityScale = 1.0f;
    bool allowRender = true;
    bool requiresEmergencyStop = false;
    std::string reason = "nominal";
    std::string authority = "canonical_safety_pipeline";
    std::uint64_t timestampNs = 0U;
};

struct SafetyDecisionTrace {
    std::vector<SafetyLayerDecision> layerDecisions;
    SafetyDecision finalDecision;

    bool auditedEveryLayer() const;
    std::string summarize() const;
};

} // namespace dnxr
