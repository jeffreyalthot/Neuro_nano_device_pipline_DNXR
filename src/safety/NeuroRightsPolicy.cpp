#include "NeuroRightsPolicy.h"

#include <algorithm>
#include <cstdint>
#include <cmath>
#include <iomanip>
#include <sstream>

namespace dnxr {
namespace {

float finiteOrZero(float value) {
    return std::isfinite(value) ? value : 0.0f;
}

bool hasText(const std::string& value) {
    return !value.empty();
}

} // namespace

NeuroRightsPolicy NeuroRightsPolicy::simulationDefault() {
    NeuroRightsPolicy policy;
    policy.setGrant({NeuroPermissionScope::NeuralReadRaw, false, "raw_neural_read_locked_by_default"});
    policy.setGrant({NeuroPermissionScope::NeuralReadFiltered, true, "filtered_signal_allowed_in_simulation"});
    policy.setGrant({NeuroPermissionScope::NeuralIntentDecode, true, "intent_decode_allowed_in_simulation"});
    policy.setGrant({NeuroPermissionScope::NeuralIntentExecute, true, "intent_execute_allowed_with_runtime_gating"});
    policy.setGrant({NeuroPermissionScope::NeuralBiometricRead, true, "biometric_fusion_allowed_in_simulation"});
    policy.setGrant({NeuroPermissionScope::XRRenderModify, true, "xr_render_modify_allowed_in_simulation"});
    policy.setGrant({NeuroPermissionScope::SafetyOverride, false, "safety_override_non_bypassable"});
    policy.setGrant({NeuroPermissionScope::SessionExport, false, "session_export_requires_explicit_grant"});
    policy.setGrant({NeuroPermissionScope::CognitiveInfluence, true, "cognitive_influence_requires_explicit_limited_consent"});
    return policy;
}

void NeuroRightsPolicy::setGrant(const NeuroPermissionGrant& grant) {
    auto existing = std::find_if(grants_.begin(), grants_.end(), [&](const NeuroPermissionGrant& item) {
        return item.scope == grant.scope;
    });
    if (existing == grants_.end()) {
        grants_.push_back(grant);
    } else {
        *existing = grant;
    }
}

NeuroRightsDecision NeuroRightsPolicy::evaluate(const NeuroRightsRequest& request) const {
    NeuroRightsDecision decision;
    decision.scopeName = scopeName(request.scope);

    const auto* grant = findGrant(request.scope);
    if (!grant || !grant->allowed) {
        decision.reason = grant ? grant->reason : "scope_not_granted";
        decision.auditTag = buildAuditTag(request, decision.reason);
        return decision;
    }

    if (!hasText(request.moduleId) || !hasText(request.traceId)) {
        decision.reason = "missing_module_or_trace";
        decision.auditTag = buildAuditTag(request, decision.reason);
        return decision;
    }

    if (request.scope == NeuroPermissionScope::SafetyOverride) {
        decision.reason = "safety_override_denied_non_bypassable";
        decision.auditTag = buildAuditTag(request, decision.reason);
        return decision;
    }

    if (request.scope == NeuroPermissionScope::NeuralReadRaw ||
        request.scope == NeuroPermissionScope::SessionExport ||
        request.scope == NeuroPermissionScope::NeuralIntentExecute) {
        if (!request.explicitConsent) {
            decision.reason = "explicit_consent_required";
            decision.auditTag = buildAuditTag(request, decision.reason);
            return decision;
        }
    }

    if (request.scope == NeuroPermissionScope::CognitiveInfluence) {
        const float requestedNorm = finiteOrZero(request.requestedInfluenceNorm);
        const float maxNorm = finiteOrZero(request.maxAllowedInfluenceNorm);
        if (!request.explicitConsent) {
            decision.reason = "cognitive_influence_requires_explicit_consent";
            decision.auditTag = buildAuditTag(request, decision.reason);
            return decision;
        }
        if (!request.simulationOnly && !request.clinicalJustification) {
            decision.reason = "cognitive_influence_requires_simulation_or_clinical_justification";
            decision.auditTag = buildAuditTag(request, decision.reason);
            return decision;
        }
        if (maxNorm <= 0.0f || requestedNorm > maxNorm || maxNorm > 0.12f) {
            decision.reason = "cognitive_influence_norm_limit_exceeded";
            decision.auditTag = buildAuditTag(request, decision.reason);
            return decision;
        }
    }

    decision.allowed = true;
    decision.reason = grant->reason;
    decision.auditTag = buildAuditTag(request, "allowed:" + decision.reason);
    return decision;
}

std::string NeuroRightsPolicy::scopeName(NeuroPermissionScope scope) {
    switch (scope) {
        case NeuroPermissionScope::NeuralReadRaw:
            return "neural.read.raw";
        case NeuroPermissionScope::NeuralReadFiltered:
            return "neural.read.filtered";
        case NeuroPermissionScope::NeuralIntentDecode:
            return "neural.intent.decode";
        case NeuroPermissionScope::NeuralIntentExecute:
            return "neural.intent.execute";
        case NeuroPermissionScope::NeuralBiometricRead:
            return "neural.biometric.read";
        case NeuroPermissionScope::XRRenderModify:
            return "xr.render.modify";
        case NeuroPermissionScope::SafetyOverride:
            return "safety.override";
        case NeuroPermissionScope::SessionExport:
            return "session.export";
        case NeuroPermissionScope::CognitiveInfluence:
            return "neural.cognitive.influence";
    }
    return "unknown";
}

bool NeuroRightsPolicy::isGranted(NeuroPermissionScope scope) const {
    const auto* grant = findGrant(scope);
    return grant && grant->allowed;
}

const NeuroPermissionGrant* NeuroRightsPolicy::findGrant(NeuroPermissionScope scope) const {
    const auto it = std::find_if(grants_.begin(), grants_.end(), [&](const NeuroPermissionGrant& grant) {
        return grant.scope == scope;
    });
    return it == grants_.end() ? nullptr : &(*it);
}

std::string NeuroRightsPolicy::buildAuditTag(
    const NeuroRightsRequest& request,
    const std::string& reason) const {

    std::uint64_t hash = 1469598103934665603ULL;
    const std::string payload = scopeName(request.scope) + "|" + request.moduleId + "|" + request.traceId + "|" + reason;
    for (unsigned char ch : payload) {
        hash ^= static_cast<std::uint64_t>(ch);
        hash *= 1099511628211ULL;
    }

    std::ostringstream out;
    out << "neurorights:" << scopeName(request.scope)
        << ":" << reason
        << ":trace=" << request.traceId
        << ":hash=" << std::hex << std::setw(16) << std::setfill('0') << hash;
    return out.str();
}

} // namespace dnxr
