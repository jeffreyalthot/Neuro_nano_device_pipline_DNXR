#include "runtime/RuntimePathLock.h"

namespace dnxr {
namespace {
bool deploymentRequestsReal(RuntimeDeploymentMode mode) {
  return mode == RuntimeDeploymentMode::HardwareBenchNoHuman ||
         mode == RuntimeDeploymentMode::PreclinicalPhantom ||
         mode == RuntimeDeploymentMode::AuthorizedResearchOnly ||
         mode == RuntimeDeploymentMode::HumanClinicalTrialAuthorized;
}
} // namespace

RuntimePathDecision RuntimePathLock::decide(
    const NeuralChipStatus &status, RuntimeDeploymentMode deploymentMode) {
  const bool requestsPrimaryReal = deploymentRequestsReal(deploymentMode) ||
                                   (!status.simulation && status.connected);

  if (decision_.lockedPrimaryReal && !requestsPrimaryReal) {
    decision_.path = RuntimePathKind::PrimaryReal;
    decision_.secondaryRuntimeAllowed = false;
    decision_.reason = "primary_real_runtime_lock_prevents_secondary_fallback";
    return decision_;
  }

  if (requestsPrimaryReal) {
    decision_.path = RuntimePathKind::PrimaryReal;
    decision_.lockedPrimaryReal = true;
    decision_.secondaryRuntimeAllowed = false;
    decision_.reason = status.simulation
                           ? "primary_real_deployment_requested"
                           : "primary_real_neural_chip_detected";
    return decision_;
  }

  decision_.path = RuntimePathKind::SecondarySimulation;
  decision_.secondaryRuntimeAllowed = true;
  decision_.reason = "secondary_simulation_runtime";
  return decision_;
}

RuntimePathDecision RuntimePathLock::current() const { return decision_; }

bool RuntimePathLock::primaryRealLocked() const {
  return decision_.lockedPrimaryReal;
}

} // namespace dnxr
