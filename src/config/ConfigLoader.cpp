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
    config.biometricSource.minimumQuality = std::clamp(config.biometricSource.minimumQuality, 0.0f, 1.0f);
    if (config.virtualWorldIngress.required && config.virtualWorldIngress.endpoint.empty()) {
        config.virtualWorldIngress.endpoint = "invalid://missing_virtual_world_ingress";
    }
    if (config.neuralChipTransport.transport.empty()) {
        config.neuralChipTransport.transport = "invalid_missing_neural_chip_transport";
    }
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
    config.runtimeMode = root.getString("runtime_mode", config.runtimeMode);
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
    if (root.has("xr_endpoint") || root.has("xr_device_id") || root.has("xr_auth_token") ||
        root.has("xr_auth_token_env") || root.has("xr_target_framerate")) {
        Logger::warn("[ConfigLoader] legacy XR headset keys are ignored by the neural-chip BCI runtime; configure observer_mirror separately");
    }
    if (const JsonValue* ingress = root.find("virtual_world_ingress"); ingress && ingress->isObject()) {
        config.virtualWorldIngress.endpoint = ingress->getString("endpoint", config.virtualWorldIngress.endpoint);
        config.virtualWorldIngress.required = ingress->getBool("required", config.virtualWorldIngress.required);
    }
    if (const JsonValue* transport = root.find("neural_chip_transport"); transport && transport->isObject()) {
        config.neuralChipTransport.transport = transport->getString("transport", config.neuralChipTransport.transport);
        config.neuralChipTransport.requireAck = transport->getBool("require_ack", config.neuralChipTransport.requireAck);
    }
    if (const JsonValue* mirror = root.find("observer_mirror"); mirror && mirror->isObject()) {
        config.observerMirror.enabled = mirror->getBool("enabled", config.observerMirror.enabled);
        config.observerMirror.endpoint = mirror->getString("endpoint", config.observerMirror.endpoint);
        config.observerMirror.failClosed = mirror->getBool("fail_closed", config.observerMirror.failClosed);
    }
    if (const JsonValue* bio = root.find("biometric_source"); bio && bio->isObject()) {
        config.biometricSource.source = bio->getString("source", config.biometricSource.source);
        config.biometricSource.required = bio->getBool("required", config.biometricSource.required);
        config.biometricSource.minimumQuality = bio->getFloat("minimum_quality", config.biometricSource.minimumQuality);
    }
    config.fullUnlockedDiagnostics = root.getBool("full_unlocked_diagnostics", config.fullUnlockedDiagnostics);
    config.replayEnabled = root.getBool("replay_enabled", config.replayEnabled);
    config.benchmarkEnabled = root.getBool("benchmark_enabled", config.benchmarkEnabled);
    config.benchmarkCycles = root.getUInt64("benchmark_cycles", config.benchmarkCycles);
    config.simulatedSignal = root.getFloatArray("simulated_signal", config.simulatedSignal);
    normalize(config);
    return config;
}

} // namespace dnxr
