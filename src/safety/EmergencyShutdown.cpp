#include "EmergencyShutdown.h"
#include "../utils/Logger.h"
namespace dnxr { void EmergencyShutdown::trigger() const { Logger::error("Emergency shutdown engaged: immersion halted."); } }
