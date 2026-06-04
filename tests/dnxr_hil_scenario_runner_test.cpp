#include "hil/HILScenarioRunner.h"

#include <cassert>

int main() {
    dnxr::HILScenario scenario;
    scenario.scenarioId = "HIL-SCEN-001";
    scenario.hazardIds = {"H-006", "H-012"};
    scenario.riskControls = {"RC-EMERGENCY-EXIT", "RC-LATENCY-BUDGET"};
    scenario.phantomProfile = "PHANTOM-VESTIBULAR-001";
    scenario.buildId = "build-0.4.0-test";
    scenario.softwareVersion = "0.4.0";
    scenario.injectedFaults = {"stimulus-timeout"};
    scenario.expectedOutcome = "safe-stop";
    scenario.emergencyStopRequired = true;
    scenario.latencyBudgetMs = 10.0;
    scenario.requiredRecoveryCycles = 3;

    dnxr::HILRunObservation observation;
    observation.observedOutcome = "safe-stop";
    observation.latencySamplesMs = {1.0, 2.0, 3.0, 4.0, 5.0};
    observation.recoveryCycles = 2;
    observation.emergencyStopPassed = true;
    observation.operatorName = "HIL operator";
    observation.reviewer = "Independent reviewer pending";
    observation.timestamp = "2026-06-04T00:00:00Z";
    observation.artifactPath = "qms/hil/session_reports/HIL-SCEN-001.md";

    const dnxr::HILScenarioRunner runner;
    const auto result = runner.run(scenario, observation);

    assert(result.passed);
    assert(result.isEvidenceComplete());
    assert(result.latency.averageMs == 3.0);
    assert(result.latency.p99Ms <= 5.0);
    return 0;
}
