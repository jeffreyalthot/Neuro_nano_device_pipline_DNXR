#pragma once

#include <string>
#include <vector>

namespace dnxr {

enum class NeuroPermissionScope {
    NeuralReadRaw,
    NeuralReadFiltered,
    NeuralIntentDecode,
    NeuralIntentExecute,
    NeuralBiometricRead,
    XRRenderModify,
    SafetyOverride,
    SessionExport,
    CognitiveInfluence
};

struct NeuroPermissionGrant {
    NeuroPermissionScope scope = NeuroPermissionScope::NeuralReadFiltered;
    bool allowed = false;
    std::string reason = "unset";
};

struct NeuroRightsRequest {
    NeuroPermissionScope scope = NeuroPermissionScope::NeuralReadFiltered;
    std::string moduleId;
    std::string traceId;
    bool explicitConsent = false;
    bool simulationOnly = true;
    bool clinicalJustification = false;
    float requestedInfluenceNorm = 0.0f;
    float maxAllowedInfluenceNorm = 0.0f;
};

struct NeuroRightsDecision {
    bool allowed = false;
    bool requiresAudit = true;
    std::string reason = "permission_denied";
    std::string scopeName;
    std::string auditTag;
};

class NeuroRightsPolicy {
public:
    static NeuroRightsPolicy simulationDefault();

    void setGrant(const NeuroPermissionGrant& grant);
    NeuroRightsDecision evaluate(const NeuroRightsRequest& request) const;

    static std::string scopeName(NeuroPermissionScope scope);
    bool isGranted(NeuroPermissionScope scope) const;

private:
    std::vector<NeuroPermissionGrant> grants_;

    const NeuroPermissionGrant* findGrant(NeuroPermissionScope scope) const;
    std::string buildAuditTag(const NeuroRightsRequest& request, const std::string& reason) const;
};

} // namespace dnxr
