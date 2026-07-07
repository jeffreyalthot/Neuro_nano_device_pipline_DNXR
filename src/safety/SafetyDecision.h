#pragma once

#include <string>

namespace dnxr {

enum class SafetyAction {
    Continue,
    ReduceIntensity,
    Decompress,
    RealityAnchor,
    EmergencyStop,
    Lockout
};

enum class SafetySeverity {
    Info,
    Warning,
    Critical
};

struct SafetyDecision {
    // Governor/runtime decision fields.
    SafetyAction action = SafetyAction::Continue;
    float riskScore = 0.0f;
    float intensityScale = 1.0f;
    bool allowRender = true;
    bool requiresEmergencyStop = false;
    std::string reason;
    std::string auditRecord;

    // Audit/trace contract fields.
    bool accepted = true;
    std::string module;
    std::string rule;
    std::string expected;
    std::string received;
    SafetySeverity severity = SafetySeverity::Info;
    std::string actionDetail;
    std::string reproduce;
    std::string correction;

    std::string explain() const;
};

} // namespace dnxr
