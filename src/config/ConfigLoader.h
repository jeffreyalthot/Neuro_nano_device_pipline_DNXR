#pragma once

#include "RuntimeConfig.h"

#include <string>

namespace dnxr {

class ConfigLoader {
public:
    static RuntimeConfig loadRuntimeConfig(const std::string& path = "config/runtime_config.json");
    static RuntimeConfig loadRuntimeConfigFromString(const std::string& json, const RuntimeConfig& base = RuntimeConfig{});
};

} // namespace dnxr
