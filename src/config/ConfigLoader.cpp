#include "ConfigLoader.h"
#include "../utils/Logger.h"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <limits>
#include <regex>
#include <sstream>

namespace dnxr {
namespace {

std::string readFile(const std::string& path) {
    std::ifstream input(path);
    if (!input) {
        return {};
    }
    std::ostringstream buffer;
    buffer << input.rdbuf();
    return buffer.str();
}

bool findString(const std::string& json, const std::string& key, std::string& out) {
    const std::regex pattern("\\\"" + key + "\\\"\\s*:\\s*\\\"([^\\\"]*)\\\"");
    std::smatch match;
    if (std::regex_search(json, match, pattern)) {
        out = match[1].str();
        return true;
    }
    return false;
}

bool findInt(const std::string& json, const std::string& key, int& out) {
    const std::regex pattern("\\\"" + key + "\\\"\\s*:\\s*(-?\\d+)");
    std::smatch match;
    if (std::regex_search(json, match, pattern)) {
        out = std::stoi(match[1].str());
        return true;
    }
    return false;
}

bool findUInt64(const std::string& json, const std::string& key, std::uint64_t& out) {
    const std::regex pattern("\\\"" + key + "\\\"\\s*:\\s*(\\d+)");
    std::smatch match;
    if (!std::regex_search(json, match, pattern)) {
        return false;
    }

    try {
        out = static_cast<std::uint64_t>(std::stoull(match[1].str()));
        return true;
    } catch (...) {
        return false;
    }
}

bool findUInt(const std::string& json, const std::string& key, std::uint32_t& out) {
    std::uint64_t value = 0U;
    if (!findUInt64(json, key, value)) {
        return false;
    }
    out = static_cast<std::uint32_t>(std::min<std::uint64_t>(value, std::numeric_limits<std::uint32_t>::max()));
    return true;
}

bool findFloat(const std::string& json, const std::string& key, float& out) {
    const std::regex pattern("\\\"" + key + "\\\"\\s*:\\s*([-+]?(?:\\d+\\.?\\d*|\\.\\d+)(?:[eE][-+]?\\d+)?)");
    std::smatch match;
    if (!std::regex_search(json, match, pattern)) {
        return false;
    }

    try {
        out = std::stof(match[1].str());
        return true;
    } catch (...) {
        return false;
    }
}

bool findBool(const std::string& json, const std::string& key, bool& out) {
    const std::regex pattern("\\\"" + key + "\\\"\\s*:\\s*(true|false)");
    std::smatch match;
    if (std::regex_search(json, match, pattern)) {
        out = (match[1].str() == "true");
        return true;
    }
    return false;
}

bool findFloatArray(const std::string& json, const std::string& key, std::vector<float>& out) {
    const std::regex pattern("\\\"" + key + "\\\"\\s*:\\s*\\[([^\\]]*)\\]");
    std::smatch match;
    if (!std::regex_search(json, match, pattern)) {
        return false;
    }

    std::vector<float> values;
    std::stringstream items(match[1].str());
    std::string item;
    while (std::getline(items, item, ',')) {
        std::stringstream valueStream(item);
        float value = 0.0f;
        if (valueStream >> value) {
            values.push_back(value);
        }
    }

    if (values.empty()) {
        return false;
    }
    out = values;
    return true;
}

void normalize(RuntimeConfig& config) {
    if (config.targetLoopMs <= 0) {
        config.targetLoopMs = 20;
    }
    if (config.maxCyclesPerSecond <= 0) {
        config.maxCyclesPerSecond = std::max(1, 1000 / config.targetLoopMs);
    }
    if (config.maxRuntimeCycles < 0) {
        config.maxRuntimeCycles = 0;
    }
    if (config.simulationChannelCount == 0U) {
        config.simulationChannelCount = 5U;
    }
    config.simulationChannelCount = std::min<std::uint32_t>(config.simulationChannelCount, 1024U);
    config.simulationNoiseAmplitude = std::clamp(config.simulationNoiseAmplitude, 0.0f, 1.0f);
    config.xrTargetFramerate = std::clamp<std::uint32_t>(config.xrTargetFramerate, 1U, 240U);
    if (config.simulatedSignal.empty()) {
        config.simulatedSignal = {0.2f, 0.4f, 0.6f, 0.3f, 0.1f};
    }
    config.dashboardEnabled = config.dashboardEnabled && !config.headless;
}

} // namespace

RuntimeConfig ConfigLoader::loadRuntimeConfig(const std::string& path) {
    const std::string json = readFile(path);
    if (json.empty()) {
        Logger::warn("[ConfigLoader] Runtime config not found, using defaults: " + path);
        return RuntimeConfig{};
    }
    Logger::info("[ConfigLoader] Loaded runtime config: " + path);
    return loadRuntimeConfigFromString(json);
}

RuntimeConfig ConfigLoader::loadRuntimeConfigFromString(const std::string& json, const RuntimeConfig& base) {
    RuntimeConfig config = base;
    findString(json, "profile", config.profile);
    findInt(json, "target_loop_ms", config.targetLoopMs);
    findInt(json, "max_cycles_per_second", config.maxCyclesPerSecond);
    findInt(json, "max_runtime_cycles", config.maxRuntimeCycles);
    findBool(json, "headless", config.headless);
    findBool(json, "dashboard_enabled", config.dashboardEnabled);
    findBool(json, "simulation_mode", config.simulationMode);
    findString(json, "signal_source", config.signalSource);
    findString(json, "replay_signal_path", config.replaySignalPath);
    findString(json, "simulation_scenario", config.simulationScenario);
    findUInt(json, "simulation_channel_count", config.simulationChannelCount);
    findFloat(json, "simulation_noise_amplitude", config.simulationNoiseAmplitude);
    findUInt64(json, "simulation_dropout_every_cycles", config.simulationDropoutEveryCycles);
    findString(json, "xr_endpoint", config.xrEndpoint);
    findString(json, "xr_device_id", config.xrDeviceId);
    findString(json, "xr_auth_token", config.xrAuthToken);
    findUInt(json, "xr_target_framerate", config.xrTargetFramerate);
    findBool(json, "xr_async_telemetry", config.xrAsyncTelemetry);
    findFloatArray(json, "simulated_signal", config.simulatedSignal);
    normalize(config);
    return config;
}

} // namespace dnxr
