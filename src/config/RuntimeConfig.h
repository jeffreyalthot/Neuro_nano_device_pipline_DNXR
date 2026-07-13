#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace dnxr {

struct VirtualWorldIngressConfig {
    std::string endpoint = "structured://local-world";
    bool required = true;
};

struct NeuralChipTransportConfig {
    std::string transport = "simulation_hal";
    bool requireAck = true;
};

struct ObserverMirrorConfig {
    bool enabled = false;
    std::string endpoint;
    bool failClosed = false;
};

struct BiometricSourceConfig {
    std::string source = "none";
    bool required = false;
    float minimumQuality = 0.0f;
};

struct RuntimeConfig {
    std::string profile = "simulation_safe";
    std::string runtimeMode = "simulation_mode";
    int targetLoopMs = 20;
    int maxCyclesPerSecond = 50;
    int maxRuntimeCycles = 0; // 0 = run until externally stopped.
    bool headless = false;
    bool dashboardEnabled = true;
    bool simulationMode = true;
    std::string signalSource = "simulated";
    std::string replaySignalPath;
    std::string simulationScenario = "nominal_focus";
    std::uint32_t simulationChannelCount = 5;
    float simulationNoiseAmplitude = 0.02f;
    std::uint64_t simulationDropoutEveryCycles = 0;

    VirtualWorldIngressConfig virtualWorldIngress;
    NeuralChipTransportConfig neuralChipTransport;
    ObserverMirrorConfig observerMirror;
    BiometricSourceConfig biometricSource;

    bool fullUnlockedDiagnostics = false;
    bool replayEnabled = false;
    bool benchmarkEnabled = false;
    std::uint64_t benchmarkCycles = 256;

    std::vector<float> simulatedSignal = {0.2f, 0.4f, 0.6f, 0.3f, 0.1f};
};

} // namespace dnxr
