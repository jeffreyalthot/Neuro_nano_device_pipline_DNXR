#include "EmergencyShutdown.h"

#include "../utils/Logger.h"

namespace dnxr {

void EmergencyShutdown::trigger() const {
    // This function emits a critical log to indicate immediate immersive shutdown.
    Logger::error("Emergency shutdown engaged: immersion halted.");
}

} // namespace dnxr
