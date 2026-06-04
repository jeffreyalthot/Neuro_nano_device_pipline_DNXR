#pragma once

#include "hil/HILScenario.h"
#include "hil/HILScenarioResult.h"

#include <string>
#include <vector>

namespace dnxr {

struct HILRunObservation {
    std::string observedOutcome;
    std::vector<double> latencySamplesMs;
    int recoveryCycles{0};
    bool emergencyStopPassed{false};
    std::string operatorName;
    std::string reviewer;
    std::string timestamp;
    std::string artifactPath;
};

class HILScenarioRunner {
public:
    HILScenarioResult run(const HILScenario& scenario, const HILRunObservation& observation) const;
};

} // namespace dnxr
