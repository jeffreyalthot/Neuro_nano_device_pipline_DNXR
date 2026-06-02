#include "../src/runtime/RuntimeSafetyStage.h"

#include <cassert>

int main() {
    dnxr::RuntimePipelineFrame frame;
    frame.cycleId = 42;
    frame.symbolicState = "focused";
    frame.sensoryFrame = {1.0f, 0.5f};
    frame.normalizedTelemetryRisk = {0.1f};
    frame.cognitive.attention = 0.8f;
    frame.cognitive.immersionIndex = 0.4f;
    frame.diagnostics.autonomyPreservation = 1.0f;

    bool recorded = false;
    auto out = dnxr::RuntimeSafetyStage::execute(frame, dnxr::RuntimeSafetyStage::Input{
        [](const dnxr::SafetyGovernorInput&) {
            dnxr::SafetyDecision decision;
            decision.reason = "nominal";
            decision.auditRecord = "audit";
            decision.allowRender = true;
            decision.intensityScale = 0.5f;
            return decision;
        },
        [&recorded](const std::string&, const std::string&, float, const dnxr::SessionAuditMetadata&) {
            recorded = true;
            dnxr::SessionAuditEvent event;
            event.eventHash = "hash";
            return event;
        },
        [](const dnxr::SafetyDecision&) { return dnxr::SafetyStateTransition{}; },
        [](const dnxr::RealityAnchorContext&, const dnxr::SafetyDecision&) {
            dnxr::RealityAnchorPlan plan;
            plan.traceTag = "anchor:maintain";
            plan.allowRender = true;
            return plan;
        },
        [](const dnxr::RealityAnchorPlan&) { return dnxr::SafetyStateTransition{}; },
        [](const std::vector<float>& sensoryFrame, const dnxr::SafetyDecision&) {
            auto out = sensoryFrame;
            out[0] *= 0.5f;
            return out;
        },
        [](const std::vector<float>& sensoryFrame, const dnxr::RealityAnchorPlan&) { return sensoryFrame; }
    });

    assert(out.continueCycle);
    assert(recorded);
    assert(out.sensoryFrame[0] == 0.5f);

    auto emergency = dnxr::RuntimeSafetyStage::execute(frame, dnxr::RuntimeSafetyStage::Input{
        [](const dnxr::SafetyGovernorInput&) {
            dnxr::SafetyDecision decision;
            decision.reason = "risk";
            decision.auditRecord = "audit";
            decision.requiresEmergencyStop = true;
            decision.allowRender = false;
            return decision;
        },
        {}, {},
        [](const dnxr::RealityAnchorContext&, const dnxr::SafetyDecision&) { return dnxr::RealityAnchorPlan{}; },
        {},
        [](const std::vector<float>& sensoryFrame, const dnxr::SafetyDecision&) { return sensoryFrame; },
        [](const std::vector<float>& sensoryFrame, const dnxr::RealityAnchorPlan&) { return sensoryFrame; }
    });
    assert(!emergency.continueCycle);
    assert(emergency.emergencyActive);
    return 0;
}
