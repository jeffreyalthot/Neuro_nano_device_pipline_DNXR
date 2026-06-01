#pragma once

#include "../config/RuntimeConfig.h"

#include <string>

namespace dnxr {

class RuntimeBootstrap {
public:
    static RuntimeConfig loadConfig(const std::string& path = "config/runtime_config.json");
};

} // namespace dnxr
