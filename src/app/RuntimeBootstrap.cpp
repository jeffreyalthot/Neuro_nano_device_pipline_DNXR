#include "RuntimeBootstrap.h"
#include "../config/ConfigLoader.h"

namespace dnxr {

RuntimeConfig RuntimeBootstrap::loadConfig(const std::string& path) {
    return ConfigLoader::loadRuntimeConfig(path);
}

} // namespace dnxr
