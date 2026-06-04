#pragma once

#include <string>
#include <vector>

namespace dnxr {

struct HILScenario {
    std::string scenarioId;
    std::vector<std::string> hazardIds;
    std::vector<std::string> riskControls;
    std::string phantomProfile;
    std::string buildId;
    std::string softwareVersion;
    std::vector<std::string> injectedFaults;
    std::string expectedOutcome;
    bool emergencyStopRequired{false};
    double latencyBudgetMs{20.0};
    int requiredRecoveryCycles{0};

    bool isProtocolBound() const;
};

} // namespace dnxr
