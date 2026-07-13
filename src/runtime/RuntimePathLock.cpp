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

RuntimePathDecision BackendAdapterLock::decide(
    const NeuralChipStatus &status, RuntimeDeploymentMode deploymentMode) {
  const bool requestsRealHardware = deploymentRequestsReal(deploymentMode) ||
                                   (!status.simulation && status.connected);

  if (decision_.lockedRealHardware && !requestsRealHardware) {
    decision_.path = RuntimePathKind::RealHardware;
    decision_.adapterFallbackAttempted = true;
    decision_.reason = "real_hardware_backend_lock_prevents_adapter_fallback";
    return decision_;
  }

  if (requestsRealHardware) {
    decision_.path = RuntimePathKind::RealHardware;
    decision_.lockedRealHardware = true;
    decision_.adapterFallbackAttempted = false;
    decision_.reason = status.simulation
                           ? "real_hardware_deployment_requested"
                           : "real_hardware_neural_chip_detected";
    return decision_;
  }

  decision_.path = RuntimePathKind::CanonicalAdapter;
  decision_.adapterFallbackAttempted = false;
  decision_.reason = "canonical_environment_adapter";
  return decision_;
}

RuntimePathDecision BackendAdapterLock::current() const { return decision_; }

bool BackendAdapterLock::realHardwareLocked() const {
  return decision_.lockedRealHardware;
}

} // namespace dnxr
