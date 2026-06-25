#include "HardwareBenchRuntime.h"
#include <iostream>
int main() {
    dnxr::RealNeuralChipHAL hal;
    const auto manifest = dnxr::HardwareBenchSession::dryRunManifest();
    const auto result = dnxr::HardwareBenchRuntime{}.startDryRun(hal, manifest);
    std::cout << result.reason << '\n';
    return result.started && !result.activeOutputSent ? 0 : 1;
}
