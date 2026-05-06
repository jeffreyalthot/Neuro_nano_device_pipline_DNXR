#include "SystemKernel.h"
#include "RuntimeController.h"
#include "../utils/Logger.h"
namespace dnxr { void SystemKernel::boot(){ Logger::info("Distributed Neural XR kernel booting (simulation reference)."); RuntimeController rc; rc.runSingleCycle(); Logger::info("Kernel cycle completed."); } }
