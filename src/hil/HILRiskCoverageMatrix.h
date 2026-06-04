#pragma once

#include "hil/HILScenario.h"
#include "hil/HILScenarioResult.h"

#include <map>
#include <string>
#include <vector>

namespace dnxr {

struct HILRiskCoverageRow {
    std::string hazardId;
    std::vector<std::string> scenarioIds;
    std::vector<std::string> riskControls;
    bool covered{false};
    bool allPassing{false};
};

class HILRiskCoverageMatrix {
public:
    void requireHazard(std::string hazardId, std::vector<std::string> riskControls);
    void addScenario(const HILScenario& scenario);
    void addResult(const HILScenarioResult& result);
    std::vector<HILRiskCoverageRow> rows() const;
    bool complete() const;
    std::string toCsv() const;

private:
    std::map<std::string, std::vector<std::string>> requiredControls_;
    std::vector<HILScenario> scenarios_;
    std::vector<HILScenarioResult> results_;
};

} // namespace dnxr
