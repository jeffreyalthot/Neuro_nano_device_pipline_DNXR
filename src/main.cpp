#include "app/RuntimeApplication.h"
#include "app/RuntimeBootstrap.h"
#include "config/RuntimeMode.h"

#include <string>

int main(int argc, char** argv) {
    dnxr::RuntimeConfig config = dnxr::RuntimeBootstrap::loadConfig();
    for (int i = 1; i < argc; ++i) {
        const std::string arg = argv[i];
        auto readValue = [&](const std::string& prefix) -> std::string {
            if (arg.rfind(prefix + "=", 0) == 0) return arg.substr(prefix.size() + 1);
            if (arg == prefix && i + 1 < argc) return argv[++i];
            return {};
        };
        const auto mode = readValue("--mode");
        if (!mode.empty()) config.runtimeMode = dnxr::toString(dnxr::runtimeModeFromString(mode));
        const auto cycles = readValue("--cycles");
        if (!cycles.empty()) config.maxRuntimeCycles = std::stoi(cycles);
        if (arg == "--headless") { config.headless = true; config.dashboardEnabled = false; }
        if (arg == "--full-unlocked") { config.runtimeMode = "full_unlocked_mode"; config.fullUnlockedDiagnostics = true; config.replayEnabled = true; config.benchmarkEnabled = true; }
        if (arg == "--benchmark") { config.runtimeMode = "benchmark_mode"; config.benchmarkEnabled = true; }
        if (arg == "--replay") { config.runtimeMode = "replay_mode"; config.replayEnabled = true; }
        if (arg == "--diagnostic") { config.runtimeMode = "diagnostic_mode"; config.fullUnlockedDiagnostics = true; }
    }
    dnxr::RuntimeApplication app(config);
    return app.run();
}
