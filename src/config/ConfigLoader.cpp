#include "ConfigLoader.h"
#include "JsonParser.h"
#include "../utils/Logger.h"

#include <algorithm>
#include <cstdint>
#include <fstream>
#include <limits>
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
    JsonValue root;
    std::string error;
    if (!JsonParser::tryParse(json, root, &error) || !root.isObject()) {
        Logger::warn("[ConfigLoader] invalid runtime config JSON, using defaults: " + error);
        normalize(config);
        return config;
    }

    config.profile = root.getString("profile", config.profile);
    config.targetLoopMs = root.getInt("target_loop_ms", config.targetLoopMs);
    config.maxCyclesPerSecond = root.getInt("max_cycles_per_second", config.maxCyclesPerSecond);
    config.maxRuntimeCycles = root.getInt("max_runtime_cycles", config.maxRuntimeCycles);
    config.headless = root.getBool("headless", config.headless);
    config.dashboardEnabled = root.getBool("dashboard_enabled", config.dashboardEnabled);
    config.simulationMode = root.getBool("simulation_mode", config.simulationMode);
    config.signalSource = root.getString("signal_source", config.signalSource);
    config.replaySignalPath = root.getString("replay_signal_path", config.replaySignalPath);
    config.simulationScenario = root.getString("simulation_scenario", config.simulationScenario);
    config.simulationChannelCount = static_cast<std::uint32_t>(std::min<std::uint64_t>(
        root.getUInt64("simulation_channel_count", config.simulationChannelCount),
        std::numeric_limits<std::uint32_t>::max()));
    config.simulationNoiseAmplitude = root.getFloat("simulation_noise_amplitude", config.simulationNoiseAmplitude);
    config.simulationDropoutEveryCycles = root.getUInt64("simulation_dropout_every_cycles", config.simulationDropoutEveryCycles);
    config.xrEndpoint = root.getString("xr_endpoint", config.xrEndpoint);
    config.xrDeviceId = root.getString("xr_device_id", config.xrDeviceId);
    if (root.has("xr_auth_token")) {
        Logger::warn("[ConfigLoader] direct xr_auth_token is refused; use xr_auth_token_env instead");
    }
    config.xrAuthTokenEnv = root.getString("xr_auth_token_env", config.xrAuthTokenEnv);
    config.xrTargetFramerate = static_cast<std::uint32_t>(std::min<std::uint64_t>(
        root.getUInt64("xr_target_framerate", config.xrTargetFramerate),
        std::numeric_limits<std::uint32_t>::max()));
    config.xrAsyncTelemetry = root.getBool("xr_async_telemetry", config.xrAsyncTelemetry);
    config.simulatedSignal = root.getFloatArray("simulated_signal", config.simulatedSignal);
    normalize(config);
    return config;
}

} // namespace dnxr
