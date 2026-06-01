#include "../src/safety/SafetyPipeline.h"

#include <cassert>
#include <string>

namespace {

dnxr::SafetyGovernorInput nominalInput() {
    dnxr::SafetyGovernorInput input{};
    input.normalizedTelemetryRisk = {0.10F, 0.12F, 0.08F};
    input.sensoryFrame = {0.20F, 0.25F, 0.18F};
    input.inferredState = "focused";
    input.cognitive.attention = 0.88F;
    input.cognitive.stress = 0.12F;
    input.cognitive.fatigue = 0.10F;
    input.diagnostics.neuralSignalQuality = 0.95F;
    input.diagnostics.autonomyPreservation = 0.96F;
    input.diagnostics.decisionDrift = 0.05F;
    input.diagnostics.addictionRisk = 0.10F;
    input.diagnostics.safetyMode = "nominal";
    return input;
}

} // namespace

int main() {
    auto pipeline = dnxr::SafetyPipeline::createDefault();
    assert(pipeline.layerCount() == 8U);

    const auto nominalTrace = pipeline.evaluate(nominalInput());
    assert(nominalTrace.layerDecisions.size() == 8U);
    assert(nominalTrace.auditedEveryLayer());
    assert(nominalTrace.finalDecision.action == dnxr::SafetyAction::Continue);
    assert(nominalTrace.finalDecision.allowRender);
    assert(!nominalTrace.finalDecision.auditRecord.empty());
    assert(nominalTrace.summarize().find("ConsentLayer") != std::string::npos);

    auto addictionInput = nominalInput();
    addictionInput.diagnostics.addictionRisk = 0.90F;
    const auto anchorTrace = pipeline.evaluate(addictionInput);
    assert(anchorTrace.finalDecision.action == dnxr::SafetyAction::RealityAnchor);
    assert(anchorTrace.finalDecision.allowRender);
    assert(anchorTrace.finalDecision.intensityScale <= 0.40F);

    auto emergencyInput = nominalInput();
    emergencyInput.inferredState = "dissociation_risk";
    emergencyInput.cognitive.stress = 0.95F;
    emergencyInput.diagnostics.decisionDrift = 0.95F;
    const auto emergencyTrace = pipeline.evaluate(emergencyInput);
    assert(emergencyTrace.finalDecision.action == dnxr::SafetyAction::EmergencyStop);
    assert(emergencyTrace.finalDecision.requiresEmergencyStop);
    assert(!emergencyTrace.finalDecision.allowRender);
    assert(emergencyTrace.auditedEveryLayer());

    dnxr::SafetyPipeline emptyPipeline;
    const auto emptyTrace = emptyPipeline.evaluate(nominalInput());
    assert(emptyTrace.layerDecisions.empty());
    assert(!emptyTrace.auditedEveryLayer());
    assert(emptyTrace.finalDecision.action == dnxr::SafetyAction::Continue);
    assert(emptyTrace.finalDecision.reason == "no_layers_configured");

    return 0;
}
