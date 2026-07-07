#include "runtime/RuntimePathLock.h"

#include <cassert>
#include <string>

int main() {
  dnxr::RuntimePathLock lock;
  dnxr::NeuralChipStatus simulated;
  simulated.connected = true;
  simulated.simulation = true;

  auto first = lock.decide(simulated, dnxr::RuntimeDeploymentMode::SimulationOnly);
  assert(first.path == dnxr::RuntimePathKind::SecondarySimulation);
  assert(first.secondaryRuntimeAllowed);
  assert(!first.lockedPrimaryReal);

  dnxr::NeuralChipStatus real;
  real.connected = true;
  real.simulation = false;
  auto primary = lock.decide(real, dnxr::RuntimeDeploymentMode::HardwareBenchNoHuman);
  assert(primary.path == dnxr::RuntimePathKind::PrimaryReal);
  assert(primary.lockedPrimaryReal);
  assert(!primary.secondaryRuntimeAllowed);

  auto noFallback = lock.decide(simulated, dnxr::RuntimeDeploymentMode::SimulationOnly);
  assert(noFallback.path == dnxr::RuntimePathKind::PrimaryReal);
  assert(noFallback.lockedPrimaryReal);
  assert(!noFallback.secondaryRuntimeAllowed);
  assert(noFallback.reason ==
         std::string("primary_real_runtime_lock_prevents_secondary_fallback"));
  return 0;
}
