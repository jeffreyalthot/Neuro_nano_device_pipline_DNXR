#include "SystemKernel.h"

#include "RuntimeController.h"
#include "../utils/Logger.h"

namespace dnxr {

void SystemKernel::boot() {
    // This function initializes the kernel context and executes a protected runtime cycle.
    Logger::info("Distributed Neural XR kernel booting (simulation reference).");

    RuntimeController controller;
    controller.runSingleCycle();

    Logger::info("Kernel cycle completed.");
}

} // namespace dnxr
