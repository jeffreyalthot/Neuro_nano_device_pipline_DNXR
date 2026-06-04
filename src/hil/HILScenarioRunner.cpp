#include "hil/HILScenarioRunner.h"

namespace dnxr {

HILScenarioResult HILScenarioRunner::run(const HILScenario& scenario, const HILRunObservation& observation) const {
    HILScenarioResult result;
    result.scenarioId = scenario.scenarioId;
    result.hazardIds = scenario.hazardIds;
    result.riskControls = scenario.riskControls;
    result.phantomProfile = scenario.phantomProfile;
    result.buildId = scenario.buildId;
    result.softwareVersion = scenario.softwareVersion;
    result.injectedFaults = scenario.injectedFaults;
    result.expectedOutcome = scenario.expectedOutcome;
    result.observedOutcome = observation.observedOutcome;
    result.latency = computeHILLatencyStatistics(observation.latencySamplesMs);
    result.recoveryCycles = observation.recoveryCycles;
    result.emergencyStopPassed = observation.emergencyStopPassed;
    result.operatorName = observation.operatorName;
    result.reviewer = observation.reviewer;
    result.timestamp = observation.timestamp;
    result.artifactPath = observation.artifactPath;

    const bool outcomeMatches = observation.observedOutcome == scenario.expectedOutcome;
    const bool latencyWithinBudget = result.latency.p99Ms <= scenario.latencyBudgetMs;
    const bool recoveredInTime = observation.recoveryCycles <= scenario.requiredRecoveryCycles;
    const bool emergencyStopOk = !scenario.emergencyStopRequired || observation.emergencyStopPassed;
    result.passed = scenario.isProtocolBound() && outcomeMatches && latencyWithinBudget && recoveredInTime && emergencyStopOk &&
                    result.isEvidenceComplete();
    return result;
}

} // namespace dnxr
