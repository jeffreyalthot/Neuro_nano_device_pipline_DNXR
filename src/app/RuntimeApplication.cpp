#include "RuntimeApplication.h"
#include "RuntimeBootstrap.h"
#include "../core/RuntimeController.h"
#include "../utils/Logger.h"

#include <utility>

namespace dnxr {

RuntimeApplication::RuntimeApplication()
    : config_(RuntimeBootstrap::loadConfig()) {}

RuntimeApplication::RuntimeApplication(RuntimeConfig config)
    : config_(std::move(config)) {}

int RuntimeApplication::run() {
    Logger::info("Booting DNXR runtime | profile=" + config_.profile + " | mode=" + config_.runtimeMode);

    RuntimeController controller(config_);
    controller.run();

    Logger::info("Shutdown complete.");
    return 0;
}

} // namespace dnxr
