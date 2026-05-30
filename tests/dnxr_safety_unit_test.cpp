#include "../src/safety/PhysiologicalGuard.h"
#include "../src/signal/EnergyThermalOptimizer.h"
#include "../src/safety/CognitiveReversibilitySystem.h"
#include "../src/safety/EthicalGuardEngine.h"
#include "../src/safety/SafetyGovernor.h"

#include <cassert>
#include <string>
#include <vector>

int main() {
    dnxr::PhysiologicalGuard guard;

    const std::vector<float> invalid = {42.0F};
    bool ok = guard.enforce(invalid);
    assert(!ok);
    assert(guard.isEmergencyActive());
    assert(guard.getEmergencyState() == dnxr::PhysiologicalGuard::EmergencyState::EmergencyLatched);
    assert(guard.getLastEmergencyReason() == "invalid_telemetry");

    const std::vector<float> nominal = {80.0F, 18.0F};
    ok = guard.enforce(nominal);
    assert(ok);
    assert(guard.getEmergencyState() == dnxr::PhysiologicalGuard::EmergencyState::Nominal);

    dnxr::EthicalGuardEngine ethical;
    const std::vector<float> normalizedTelemetry = {0.15F, 0.2F, 0.1F};
    assert(ethical.enforceNonBypassableLimits(normalizedTelemetry, "focused"));
    assert(!ethical.enforceNonBypassableLimits({0.95F, 0.9F}, "dissociation_risk"));
    assert(ethical.signedAuditRecord("safety_unit", normalizedTelemetry).find("sig=") != std::string::npos);

    dnxr::CognitiveReversibilitySystem reversibility;
    assert(reversibility.shouldStartReturnToReality({0.8F, 0.82F, 0.78F}));
    const auto decompressed = reversibility.runDecompression({1.0F, -1.0F, 0.4F});
    assert(decompressed[0] <= 0.65F);
    assert(decompressed[1] >= -0.65F);

    dnxr::EnergyThermalOptimizer optimizer;
    const auto optimized = optimizer.optimize({1.2F, -1.2F, 0.5F}, 0.25F);
    assert(optimized[0] <= 1.0F);
    assert(optimized[1] >= -1.0F);
    assert(optimized[2] > 0.0F);

    dnxr::SafetyGovernor governor;
    dnxr::SafetyGovernorInput nominalInput{};
    nominalInput.normalizedTelemetryRisk = {0.1F, 0.1F, 0.1F};
    nominalInput.sensoryFrame = {0.25F, 0.3F, 0.2F};
    nominalInput.inferredState = "focused";
    nominalInput.cognitive.attention = 0.85F;
    nominalInput.cognitive.stress = 0.15F;
    nominalInput.cognitive.fatigue = 0.10F;
    nominalInput.diagnostics.neuralSignalQuality = 0.95F;
    nominalInput.diagnostics.autonomyPreservation = 0.96F;
    const auto nominalDecision = governor.evaluate(nominalInput);
    assert(nominalDecision.action == dnxr::SafetyAction::Continue);
    assert(nominalDecision.allowRender);

    dnxr::SafetyGovernorInput emergencyInput = nominalInput;
    emergencyInput.inferredState = "dissociation_risk";
    emergencyInput.diagnostics.decisionDrift = 0.95F;
    const auto emergencyDecision = governor.evaluate(emergencyInput);
    assert(emergencyDecision.requiresEmergencyStop);
    assert(!emergencyDecision.allowRender);
    assert(!emergencyDecision.auditRecord.empty());

    const auto governed = governor.applyDecisionToFrame({1.0F, 0.5F}, emergencyDecision);
    assert(governed[0] == 0.0F);
    assert(governed[1] == 0.0F);

    return 0;
}
