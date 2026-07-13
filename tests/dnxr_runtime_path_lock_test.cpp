#include "runtime/RuntimePathLock.h"

#include <cassert>
#include <string>

int main() {
  dnxr::BackendAdapterLock lock;
  dnxr::NeuralChipStatus simulated;
  simulated.connected = true;
  simulated.simulation = true;

  auto first = lock.decide(simulated, dnxr::RuntimeDeploymentMode::SimulationOnly);
  assert(first.path == dnxr::RuntimePathKind::CanonicalAdapter);
  assert(!first.adapterFallbackAttempted);
  assert(!first.lockedRealHardware);

  dnxr::NeuralChipStatus real;
  real.connected = true;
  real.simulation = false;
  auto primary = lock.decide(real, dnxr::RuntimeDeploymentMode::HardwareBenchNoHuman);
  assert(primary.path == dnxr::RuntimePathKind::RealHardware);
  assert(primary.lockedRealHardware);
  assert(!primary.adapterFallbackAttempted);

  auto noFallback = lock.decide(simulated, dnxr::RuntimeDeploymentMode::SimulationOnly);
  assert(noFallback.path == dnxr::RuntimePathKind::RealHardware);
  assert(noFallback.lockedRealHardware);
  assert(noFallback.adapterFallbackAttempted);
  assert(noFallback.reason ==
         std::string("real_hardware_backend_lock_prevents_adapter_fallback"));
  return 0;
}
