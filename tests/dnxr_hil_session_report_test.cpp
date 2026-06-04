#include "hil/HILSessionReport.h"

#include <cassert>
#include <string>

int main() {
    dnxr::HILScenarioResult result;
    result.scenarioId = "HIL-SCEN-REPORT";
    result.hazardIds = {"H-003"};
    result.riskControls = {"RC-VESTIBULAR-BUDGET"};
    result.phantomProfile = "PHANTOM-VESTIBULAR-001";
    result.buildId = "build-0.4.0-test";
    result.softwareVersion = "0.4.0";
    result.injectedFaults = {"vestibular-conflict"};
    result.expectedOutcome = "stimulus-limited";
    result.observedOutcome = "stimulus-limited";
    result.passed = true;
    result.latency = {4.0, 5.0, 6.0, 7.0};
    result.recoveryCycles = 1;
    result.emergencyStopPassed = true;
    result.operatorName = "HIL operator";
    result.reviewer = "Independent reviewer pending";
    result.timestamp = "2026-06-04T00:00:00Z";
    result.artifactPath = "qms/hil/session_reports/HIL-SCEN-REPORT.md";

    dnxr::HILSessionReport report;
    report.addResult(result);
    const std::string markdown = report.toMarkdown();

    assert(report.allPassed());
    assert(markdown.find("Scenario ID") != std::string::npos);
    assert(markdown.find("p99 latency ms") != std::string::npos);
    assert(markdown.find("Emergency-stop result") != std::string::npos);
    assert(markdown.find("qms/hil/session_reports/HIL-SCEN-REPORT.md") != std::string::npos);
    return 0;
}
