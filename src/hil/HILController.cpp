#include "HILController.h"

namespace dnxr {

HILSessionResult HILController::runSession(const HardwarePhantomProfile& p,
                                           const FaultInjectionEngine& f,
                                           const LatencyInstrumentation& l) const {
    HILSessionResult r;
    const auto lr = l.report(p.latencyBudgetMs);
    r.emergencyStopped = f.hasFault(HILFaultType::EmergencyStop) || f.hasFault(HILFaultType::PhantomDisconnect);
    r.safetyResponse = r.emergencyStopped || !lr.withinBudget || !p.connected || f.hasFault(HILFaultType::LatencyOverrun);
    r.recovered = r.safetyResponse;
    r.passed = p.connected && lr.withinBudget && !f.hasFault(HILFaultType::ReplayAttack) &&
               !f.hasFault(HILFaultType::PacketCorruption);
    r.summary = r.passed ? "HIL session passed" : "HIL session produced safety evidence with gaps";
    return r;
}

} // namespace dnxr
