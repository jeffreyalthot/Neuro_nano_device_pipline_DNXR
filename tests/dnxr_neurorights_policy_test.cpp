#include "../src/safety/NeuroRightsPolicy.h"

#include <iostream>
#include <string>

namespace {

bool require(bool condition, const std::string& message) {
    if (!condition) {
        std::cerr << "dnxr_neurorights_policy_test: " << message << '\n';
        return false;
    }
    return true;
}

dnxr::NeuroRightsRequest request(dnxr::NeuroPermissionScope scope) {
    dnxr::NeuroRightsRequest req;
    req.scope = scope;
    req.moduleId = "unit_test_module";
    req.traceId = "trace_001";
    req.simulationOnly = true;
    return req;
}

} // namespace

int main() {
    auto policy = dnxr::NeuroRightsPolicy::simulationDefault();

    auto filtered = request(dnxr::NeuroPermissionScope::NeuralReadFiltered);
    const auto filteredDecision = policy.evaluate(filtered);
    if (!require(filteredDecision.allowed, "filtered neural reads should be allowed in simulation") ||
        !require(filteredDecision.scopeName == "neural.read.filtered", "filtered scope should have stable name") ||
        !require(filteredDecision.auditTag.find("trace_001") != std::string::npos, "audit tag should include trace id")) {
        return 1;
    }

    auto raw = request(dnxr::NeuroPermissionScope::NeuralReadRaw);
    raw.explicitConsent = true;
    const auto rawDecision = policy.evaluate(raw);
    if (!require(!rawDecision.allowed, "raw neural reads should remain locked without explicit grant") ||
        !require(rawDecision.reason == "raw_neural_read_locked_by_default", "raw denial should be explicit")) {
        return 1;
    }

    auto execute = request(dnxr::NeuroPermissionScope::NeuralIntentExecute);
    const auto noConsentExecute = policy.evaluate(execute);
    execute.explicitConsent = true;
    const auto consentExecute = policy.evaluate(execute);
    if (!require(!noConsentExecute.allowed, "intent execution should require explicit consent") ||
        !require(consentExecute.allowed, "intent execution should allow after explicit consent and grant")) {
        return 1;
    }

    auto influence = request(dnxr::NeuroPermissionScope::CognitiveInfluence);
    influence.explicitConsent = true;
    influence.requestedInfluenceNorm = 0.10F;
    influence.maxAllowedInfluenceNorm = 0.12F;
    const auto influenceAllowed = policy.evaluate(influence);
    influence.requestedInfluenceNorm = 0.13F;
    const auto influenceDenied = policy.evaluate(influence);
    if (!require(influenceAllowed.allowed, "bounded simulation influence should be allowed with explicit consent") ||
        !require(!influenceDenied.allowed, "influence above max norm should be denied") ||
        !require(influenceDenied.reason == "cognitive_influence_norm_limit_exceeded", "influence denial reason should mention norm")) {
        return 1;
    }

    auto safetyOverride = request(dnxr::NeuroPermissionScope::SafetyOverride);
    safetyOverride.explicitConsent = true;
    safetyOverride.clinicalJustification = true;
    const auto safetyDecision = policy.evaluate(safetyOverride);
    if (!require(!safetyDecision.allowed, "safety override must remain non-bypassable") ||
        !require(safetyDecision.scopeName == "safety.override", "safety override scope should be stable")) {
        return 1;
    }

    auto missingTrace = request(dnxr::NeuroPermissionScope::NeuralReadFiltered);
    missingTrace.traceId.clear();
    const auto missingTraceDecision = policy.evaluate(missingTrace);
    if (!require(!missingTraceDecision.allowed, "requests without trace ids should be denied") ||
        !require(missingTraceDecision.reason == "missing_module_or_trace", "missing trace should be explicit")) {
        return 1;
    }

    return 0;
}
