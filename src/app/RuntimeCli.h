#pragma once

#include "../config/RuntimeConfig.h"

#include <string>
#include <vector>

namespace dnxr {

enum class RuntimeCliAction {
    Help,
    Version,
    ValidateConfig,
    Run,
    Error
};

struct RuntimeCliResult {
    RuntimeCliAction action = RuntimeCliAction::Error;
    RuntimeConfig config{};
    std::string configPath = "config/runtime_config.json";
    std::string message;
    int exitCode = 2;
};

class RuntimeCli {
public:
    static RuntimeCliResult parse(int argc, char** argv);
    static std::string helpText();
    static std::string versionText();
};

} // namespace dnxr
