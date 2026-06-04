#include "hil/HILRiskCoverageMatrix.h"

#include <cassert>
#include <string>

int main() {
    dnxr::HILScenario scenario;
    scenario.scenarioId = "HIL-SCEN-COVERAGE";
    scenario.hazardIds = {"H-006"};
    scenario.riskControls = {"RC-EMERGENCY-EXIT"};

    dnxr::HILScenarioResult result;
    result.scenarioId = scenario.scenarioId;
    result.hazardIds = scenario.hazardIds;
    result.riskControls = scenario.riskControls;
    result.passed = true;

    dnxr::HILRiskCoverageMatrix matrix;
    matrix.requireHazard("H-006", {"RC-EMERGENCY-EXIT"});
    matrix.addScenario(scenario);
    matrix.addResult(result);

    const auto rows = matrix.rows();
    assert(rows.size() == 1U);
    assert(rows[0].covered);
    assert(rows[0].allPassing);
    assert(matrix.complete());
    assert(matrix.toCsv().find("H-006") != std::string::npos);
    return 0;
}
