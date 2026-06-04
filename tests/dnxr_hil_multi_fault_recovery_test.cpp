#include "hil/HILScenarioRunner.h"

#include <cassert>

int main() {
    dnxr::HILScenario scenario;
    scenario.scenarioId = "HIL-SCEN-MULTI-FAULT";
    scenario.hazardIds = {"H-006", "H-010", "H-012"};
    scenario.riskControls = {"RC-EMERGENCY-EXIT", "RC-CALIBRATION-DRIFT", "RC-WCET"};
    scenario.phantomProfile = "PHANTOM-MULTI-FAULT-001";
    scenario.buildId = "build-0.4.0-test";
    scenario.softwareVersion = "0.4.0";
    scenario.injectedFaults = {"phantom-disconnect", "calibration-drift", "timing-overrun"};
    scenario.expectedOutcome = "safe-recovered";
    scenario.emergencyStopRequired = true;
    scenario.latencyBudgetMs = 15.0;
    scenario.requiredRecoveryCycles = 5;

    dnxr::HILRunObservation observation;
    observation.observedOutcome = "safe-recovered";
    observation.latencySamplesMs = {5.0, 7.0, 8.0, 9.0, 10.0};
    observation.recoveryCycles = 4;
    observation.emergencyStopPassed = true;
    observation.operatorName = "HIL operator";
    observation.reviewer = "Independent reviewer pending";
    observation.timestamp = "2026-06-04T00:00:00Z";
    observation.artifactPath = "qms/hil/session_reports/HIL-SCEN-MULTI-FAULT.md";

    const auto result = dnxr::HILScenarioRunner().run(scenario, observation);
    assert(result.passed);
    assert(result.injectedFaults.size() == 3U);
    assert(result.recoveryCycles == 4);
    return 0;
}
