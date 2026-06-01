#include "../src/hardware/NeuralChipHAL.h"

#include <cassert>
#include <iostream>
#include <stdexcept>

int main() {
#if DNXR_SIMULATION_ONLY
    bool rejectedHardwareInLoop = false;
    try {
        dnxr::SimulatedNeuralChipHAL hardwareMode(
            8,
            dnxr::NeuralChipRuntimeMode::HardwareInLoop);
    } catch (const std::runtime_error&) {
        rejectedHardwareInLoop = true;
    }
    assert(rejectedHardwareInLoop);

    bool rejectedClinical = false;
    try {
        dnxr::SimulatedNeuralChipHAL clinicalMode(
            8,
            dnxr::NeuralChipRuntimeMode::ClinicalLocked);
    } catch (const std::runtime_error&) {
        rejectedClinical = true;
    }
    assert(rejectedClinical);
#endif

    dnxr::SimulatedNeuralChipHAL simulationMode(8);
    simulationMode.connect("simulation_only_guard_chip");
    assert(simulationMode.status().connected);
    assert(simulationMode.status().simulation);

    std::cout << "Simulation-only neural chip guard OK\n";
    return 0;
}
