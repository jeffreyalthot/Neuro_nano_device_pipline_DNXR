#include "hil/HILRiskCoverageMatrix.h"

#include <set>
#include <sstream>
#include <utility>

namespace dnxr {

namespace {
std::string join(const std::vector<std::string>& values) {
    std::ostringstream out;
    for (std::size_t i = 0; i < values.size(); ++i) {
        if (i != 0U) {
            out << ";";
        }
        out << values[i];
    }
    return out.str();
}
} // namespace

void HILRiskCoverageMatrix::requireHazard(std::string hazardId, std::vector<std::string> riskControls) {
    requiredControls_[std::move(hazardId)] = std::move(riskControls);
}

void HILRiskCoverageMatrix::addScenario(const HILScenario& scenario) { scenarios_.push_back(scenario); }

void HILRiskCoverageMatrix::addResult(const HILScenarioResult& result) { results_.push_back(result); }

std::vector<HILRiskCoverageRow> HILRiskCoverageMatrix::rows() const {
    std::vector<HILRiskCoverageRow> rows;
    for (const auto& hazard : requiredControls_) {
        HILRiskCoverageRow row;
        row.hazardId = hazard.first;
        row.riskControls = hazard.second;
        std::set<std::string> scenarioIds;
        std::set<std::string> passingScenarioIds;
        for (const auto& scenario : scenarios_) {
            for (const auto& scenarioHazard : scenario.hazardIds) {
                if (scenarioHazard == hazard.first) {
                    scenarioIds.insert(scenario.scenarioId);
                }
            }
        }
        for (const auto& result : results_) {
            for (const auto& resultHazard : result.hazardIds) {
                if (resultHazard == hazard.first && result.passed) {
                    passingScenarioIds.insert(result.scenarioId);
                }
            }
        }
        row.scenarioIds.assign(scenarioIds.begin(), scenarioIds.end());
        row.covered = !row.scenarioIds.empty();
        row.allPassing = row.covered && passingScenarioIds.size() == scenarioIds.size();
        rows.push_back(row);
    }
    return rows;
}

bool HILRiskCoverageMatrix::complete() const {
    const auto matrixRows = rows();
    if (matrixRows.empty()) {
        return false;
    }
    for (const auto& row : matrixRows) {
        if (!row.covered || !row.allPassing) {
            return false;
        }
    }
    return true;
}

std::string HILRiskCoverageMatrix::toCsv() const {
    std::ostringstream out;
    out << "HazardID,ScenarioIDs,RiskControls,Covered,AllPassing\n";
    for (const auto& row : rows()) {
        out << row.hazardId << "," << join(row.scenarioIds) << "," << join(row.riskControls) << ","
            << (row.covered ? "true" : "false") << "," << (row.allPassing ? "true" : "false") << "\n";
    }
    return out.str();
}

} // namespace dnxr
