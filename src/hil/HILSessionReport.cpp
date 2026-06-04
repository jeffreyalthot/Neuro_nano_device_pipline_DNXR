#include "hil/HILSessionReport.h"

#include <sstream>

namespace dnxr {

namespace {
std::string join(const std::vector<std::string>& values) {
    std::ostringstream out;
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i != 0U) {
            out << "; ";
        }
        out << values[i];
    }
    return out.str();
}
} // namespace

void HILSessionReport::addResult(const HILScenarioResult& result) { results_.push_back(result); }

std::vector<HILScenarioResult> HILSessionReport::results() const { return results_; }

bool HILSessionReport::allPassed() const {
    if (results_.empty()) {
        return false;
    }
    for (const auto& result : results_) {
        if (!result.passed) {
            return false;
        }
    }
    return true;
}

std::string HILSessionReport::toMarkdown() const {
    std::ostringstream out;
    out << "# HIL Session Report\n\n";
    out << "| Scenario ID | Hazard IDs | Risk controls | Phantom profile | Build ID | Software version | "
           "Injected faults | Expected outcome | Observed outcome | Pass/fail | Average latency ms | p95 latency ms | "
           "p99 latency ms | Worst latency ms | Recovery cycles | Emergency-stop result | Operator | Reviewer | "
           "Timestamp | Artifact path |\n";
    out << "| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |\n";
    for (const auto& result : results_) {
        out << "| " << result.scenarioId << " | " << join(result.hazardIds) << " | " << join(result.riskControls) << " | "
            << result.phantomProfile << " | " << result.buildId << " | " << result.softwareVersion << " | "
            << join(result.injectedFaults) << " | " << result.expectedOutcome << " | " << result.observedOutcome << " | "
            << (result.passed ? "PASS" : "FAIL") << " | " << result.latency.averageMs << " | " << result.latency.p95Ms
            << " | " << result.latency.p99Ms << " | " << result.latency.worstMs << " | " << result.recoveryCycles << " | "
            << (result.emergencyStopPassed ? "PASS" : "NOT REQUIRED/FAIL") << " | " << result.operatorName << " | "
            << result.reviewer << " | " << result.timestamp << " | " << result.artifactPath << " |\n";
    }
    out << "\nOverall status: " << (allPassed() ? "PASS" : "NOT READY") << "\n";
    return out.str();
}

} // namespace dnxr
