#pragma once

#include <string>
#include <vector>

namespace dnxr {

struct HILLatencyStatistics {
    double averageMs{0.0};
    double p95Ms{0.0};
    double p99Ms{0.0};
    double worstMs{0.0};
};

struct HILScenarioResult {
    std::string scenarioId;
    std::vector<std::string> hazardIds;
    std::vector<std::string> riskControls;
    std::string phantomProfile;
    std::string buildId;
    std::string softwareVersion;
    std::vector<std::string> injectedFaults;
    std::string expectedOutcome;
    std::string observedOutcome;
    bool passed{false};
    HILLatencyStatistics latency;
    int recoveryCycles{0};
    bool emergencyStopPassed{false};
    std::string operatorName;
    std::string reviewer;
    std::string timestamp;
    std::string artifactPath;

    bool isEvidenceComplete() const;
};

HILLatencyStatistics computeHILLatencyStatistics(std::vector<double> samplesMs);

} // namespace dnxr
