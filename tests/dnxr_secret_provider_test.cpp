#include "../src/security/SecretProvider.h"

#include <cassert>
#include <cstdlib>
#include <iostream>

int main() {
    unsetenv("DNXR_UNIT_SECRET");

    const dnxr::RuntimeSecret missing = dnxr::SecretProvider::fromEnvironment(
        "unit_secret",
        "DNXR_UNIT_SECRET");
    assert(missing.empty());

    const dnxr::RuntimeSecret simulatedA = dnxr::SecretProvider::deterministicSimulationSecret(
        "unit_secret",
        "session-a");
    const dnxr::RuntimeSecret simulatedB = dnxr::SecretProvider::deterministicSimulationSecret(
        "unit_secret",
        "session-a");
    assert(simulatedA.origin == dnxr::SecretOrigin::SimulationDeterministic);
    assert(!simulatedA.empty());
    assert(simulatedA.value == simulatedB.value);
    assert(simulatedA.value.rfind("sim-", 0) == 0);

    setenv("DNXR_UNIT_SECRET", "unit-env-secret", 1);
    const dnxr::RuntimeSecret envSecret = dnxr::SecretProvider::resolve(
        "unit_secret",
        "DNXR_UNIT_SECRET");
    assert(envSecret.origin == dnxr::SecretOrigin::Environment);
    assert(envSecret.value == "unit-env-secret");

    unsetenv("DNXR_UNIT_SECRET");
    std::cout << "Secret provider env and simulation fallback OK\n";
    return 0;
}
