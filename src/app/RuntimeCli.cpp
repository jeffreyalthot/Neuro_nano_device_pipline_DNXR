#include "RuntimeCli.h"
#include "RuntimeBootstrap.h"
#include "../config/RuntimeMode.h"

#include <stdexcept>

namespace dnxr {
namespace {

bool isOption(const std::string& value) {
    return value.rfind("--", 0) == 0;
}

bool readValue(int argc, char** argv, int& index, const std::string& option, std::string& out, std::string& error) {
    const std::string arg = argv[index];
    if (arg.rfind(option + "=", 0) == 0) {
        out = arg.substr(option.size() + 1);
        if (out.empty()) {
            error = option + " requires a non-empty value";
            return false;
        }
        return true;
    }
    if (arg == option) {
        if (index + 1 >= argc || isOption(argv[index + 1])) {
            error = option + " requires a value";
            return false;
        }
        out = argv[++index];
        return true;
    }
    return false;
}

RuntimeCliResult errorResult(const std::string& message) {
    RuntimeCliResult result;
    result.action = RuntimeCliAction::Error;
    result.message = message;
    result.exitCode = 2;
    return result;
}

} // namespace

std::string RuntimeCli::helpText() {
    return "DNXR runtime\n"
           "Usage:\n"
           "  distributed_neural_xr --help\n"
           "  distributed_neural_xr --version\n"
           "  distributed_neural_xr validate-config --config <path>\n"
           "  distributed_neural_xr run --profile <profile> --config <path> [--cycles <n>] [--headless]\n"
           "\n"
           "No command starts a neural cycle. Active startup is explicit via run.\n"
           "Non-real profiles are labelled in the startup banner and session digest.\n";
}

std::string RuntimeCli::versionText() {
    return "DNXR runtime 0.0-forensic-baseline (not for human use)";
}

RuntimeCliResult RuntimeCli::parse(int argc, char** argv) {
    RuntimeCliResult result;
    if (argc <= 1) {
        return errorResult("missing command; use --help or run --profile <profile> --config <path>");
    }

    const std::string command = argv[1];
    if (command == "--help" || command == "-h") {
        result.action = RuntimeCliAction::Help;
        result.message = helpText();
        result.exitCode = 0;
        return result;
    }
    if (command == "--version") {
        result.action = RuntimeCliAction::Version;
        result.message = versionText();
        result.exitCode = 0;
        return result;
    }
    if (isOption(command)) {
        return errorResult("unknown option before command: " + command);
    }
    if (command != "run" && command != "validate-config") {
        return errorResult("unknown command: " + command);
    }

    std::string configPath;
    std::string profile;
    int cycles = -1;
    bool headless = false;
    bool dashboardEnabled = true;
    std::string runtimeMode;
    bool diagnostics = false;
    bool benchmark = false;
    bool replay = false;

    for (int i = 2; i < argc; ++i) {
        const std::string arg = argv[i];
        std::string value;
        std::string error;
        if (readValue(argc, argv, i, "--config", value, error)) {
            configPath = value;
        } else if (!error.empty()) {
            return errorResult(error);
        } else if (readValue(argc, argv, i, "--profile", value, error)) {
            profile = value;
        } else if (!error.empty()) {
            return errorResult(error);
        } else if (readValue(argc, argv, i, "--mode", value, error)) {
            runtimeMode = value;
        } else if (!error.empty()) {
            return errorResult(error);
        } else if (readValue(argc, argv, i, "--cycles", value, error)) {
            try {
                cycles = std::stoi(value);
            } catch (const std::exception&) {
                return errorResult("--cycles must be an integer");
            }
        } else if (!error.empty()) {
            return errorResult(error);
        } else if (arg == "--headless") {
            headless = true;
            dashboardEnabled = false;
        } else if (arg == "--diagnostic" || arg == "--full-unlocked") {
            diagnostics = true;
            runtimeMode = "diagnostic_mode";
        } else if (arg == "--benchmark") {
            benchmark = true;
            runtimeMode = "benchmark_mode";
        } else if (arg == "--replay") {
            replay = true;
            runtimeMode = "replay_mode";
        } else {
            return errorResult("unknown argument: " + arg);
        }
    }

    if (configPath.empty()) {
        return errorResult(command + " requires --config <path>");
    }

    result.configPath = configPath;
    try {
        result.config = RuntimeBootstrap::loadConfig(configPath);
    } catch (const std::exception& ex) {
        return errorResult("failed to load config: " + std::string(ex.what()));
    }

    if (command == "validate-config") {
        result.action = RuntimeCliAction::ValidateConfig;
        result.message = "config valid: " + configPath;
        result.exitCode = 0;
        return result;
    }

    if (profile.empty()) {
        return errorResult("run requires --profile <profile>");
    }
    result.config.profile = profile;
    if (!runtimeMode.empty()) {
        result.config.runtimeMode = toString(runtimeModeFromString(runtimeMode));
    }
    if (cycles >= 0) {
        result.config.maxRuntimeCycles = cycles;
    }
    result.config.headless = headless;
    result.config.dashboardEnabled = dashboardEnabled;
    result.config.fullUnlockedDiagnostics = diagnostics;
    result.config.benchmarkEnabled = benchmark;
    result.config.replayEnabled = replay;
    result.action = RuntimeCliAction::Run;
    result.message = "run requested";
    result.exitCode = 0;
    return result;
}

} // namespace dnxr
