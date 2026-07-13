#include "../src/config/ConfigLoader.h"

#include <cassert>
#include <cstdint>
#include <string>

int main() {
    const std::string json = R"json({
        "profile": "latency_benchmark",
        "target_loop_ms": -3,
        "max_cycles_per_second": -1,
        "max_runtime_cycles": -500,
        "headless": true,
        "dashboard_enabled": true,
        "simulation_mode": true,
        "signal_source": "file_replay",
        "replay_signal_path": "replays/session_001.dnxr_replay",
        "simulation_scenario": "latency_spike",
        "simulation_channel_count": 2048,
        "simulation_noise_amplitude": 1.25e0,
        "simulation_dropout_every_cycles": 4294967297,
        "virtual_world_ingress": {"endpoint": "structured://world.test", "required": true},
        "neural_chip_transport": {"transport": "bench_transport", "require_ack": true},
        "observer_mirror": {"enabled": false, "endpoint": "observer://off", "fail_closed": false},
        "biometric_source": {"source": "certified_monitor", "required": true, "minimum_quality": 1.25},
        "simulated_signal": [0.1, 0.2, 0.3]
    })json";

    const dnxr::RuntimeConfig config = dnxr::ConfigLoader::loadRuntimeConfigFromString(json);
    assert(config.profile == "latency_benchmark");
    assert(config.targetLoopMs == 20);
    assert(config.maxCyclesPerSecond == 50);
    assert(config.maxRuntimeCycles == 0);
    assert(config.headless);
    assert(!config.dashboardEnabled);
    assert(config.signalSource == "file_replay");
    assert(config.replaySignalPath == "replays/session_001.dnxr_replay");
    assert(config.simulationScenario == "latency_spike");
    assert(config.simulationChannelCount == 1024U);
    assert(config.simulationNoiseAmplitude == 1.0f);
    assert(config.simulationDropoutEveryCycles == 4294967297ULL);
    assert(config.virtualWorldIngress.endpoint == "structured://world.test");
    assert(config.neuralChipTransport.transport == "bench_transport");
    assert(!config.observerMirror.enabled);
    assert(config.biometricSource.source == "certified_monitor");
    assert(config.biometricSource.required);
    assert(config.biometricSource.minimumQuality == 1.0f);
    assert(config.simulatedSignal.size() == 3U);
    assert(config.simulatedSignal[2] == 0.3f);

    dnxr::RuntimeConfig base;
    base.profile = "developer_debug";
    base.targetLoopMs = 8;
    base.maxCyclesPerSecond = 125;
    base.simulatedSignal = {0.9f};
    const dnxr::RuntimeConfig partial = dnxr::ConfigLoader::loadRuntimeConfigFromString(
        R"json({"simulation_noise_amplitude": -0.5})json",
        base);
    assert(partial.profile == "developer_debug");
    assert(partial.targetLoopMs == 8);
    assert(partial.maxCyclesPerSecond == 125);
    assert(partial.simulationNoiseAmplitude == 0.0f);
    assert(partial.simulatedSignal.size() == 1U);
    assert(partial.simulatedSignal[0] == 0.9f);

    const dnxr::RuntimeConfig defaults = dnxr::ConfigLoader::loadRuntimeConfig("build/does_not_exist_runtime_config.json");
    assert(defaults.profile == "simulation_safe");
    assert(defaults.targetLoopMs == 20);
    assert(defaults.simulatedSignal.size() == 5U);

    return 0;
}
