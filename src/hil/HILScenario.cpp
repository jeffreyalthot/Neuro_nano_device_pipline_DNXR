#include "hil/HILScenario.h"

namespace dnxr {

bool HILScenario::isProtocolBound() const {
    return !scenarioId.empty() && !hazardIds.empty() && !riskControls.empty() && !phantomProfile.empty() &&
           !buildId.empty() && !softwareVersion.empty() && !expectedOutcome.empty() && latencyBudgetMs > 0.0;
}

} // namespace dnxr
