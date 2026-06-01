#pragma once

#include <cstdint>
#include <string>
#include <vector>

namespace dnxr {

struct RuntimeConfig {
    std::string profile = "simulation_safe";
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

    std::string xrEndpoint = "wss://xr.local";
    std::string xrDeviceId = "demo-device";
    std::string xrAuthTokenEnv = "DNXR_XR_AUTH_TOKEN";
    std::uint32_t xrTargetFramerate = 90;
    bool xrAsyncTelemetry = true;

    std::vector<float> simulatedSignal = {0.2f, 0.4f, 0.6f, 0.3f, 0.1f};
};

} // namespace dnxr
