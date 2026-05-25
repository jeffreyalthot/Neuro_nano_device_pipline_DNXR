#pragma once

#include <string>

namespace dnxr {

struct AddictionRiskReport {
    float instabilityExposure = 0.0f;
    float emotionalVolatility = 0.0f;
    float addictionRisk = 0.0f;
    bool mitigationActive = false;
    std::string policyAction;
};

class AddictionRiskGuard {
public:
    AddictionRiskReport evaluate(float sceneInstability,
                                 float emotionalValence,
                                 float stress,
                                 float sessionMinutes) const;
};

} // namespace dnxr
