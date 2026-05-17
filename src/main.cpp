// main.cpp

#include <iostream>
#include <iomanip>
#include <thread>
#include <chrono>
#include <atomic>
#include <vector>
#include <string>

#include "core/SystemKernel.h"
#include "utils/Logger.h"
#include "utils/RuntimeMetrics.h"

#include "signal/SignalStreamProcessor.h"
#include "signal/FeatureVectorizer.h"
#include "cognition/StateInferenceEngine.h"
#include "calibration/NeuralCalibrationEngine.h"
#include "immersion/ImmersionCore.h"
#include "rendering/XRRenderer.h"

#include "safety/PhysiologicalGuard.h"
#include "safety/EmergencyShutdown.h"
#include "safety/StabilityMonitor.h"

#include "devices/XRHeadsetInterface.h"
#include "devices/NanoNodeInterface.h"
#include "devices/HostComputerBridge.h"

using namespace dnxr;

namespace {

std::atomic<bool> running{true};

void clearScreen() {
#ifdef _WIN32
    system("cls");
#else
    system("clear");
#endif
}

void drawHeader() {
    std::cout
        << "=============================================\n"
        << "   DNXR NEURAL XR RUNTIME DASHBOARD (CLI)\n"
        << "=============================================\n\n";
}

void drawSection(const std::string& title) {
    std::cout << "\n[" << title << "]\n";
    std::cout << "---------------------------------------------\n";
}

void sleepMs(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}

} // namespace

int main() {
    constexpr int kTargetLoopMs = 20;

    Logger::info("Booting DNXR runtime...");

    SystemKernel kernel;
    kernel.setCycleDuration(kTargetLoopMs);
    kernel.setMaxCyclesPerSecond(1000 / kTargetLoopMs);
    kernel.boot();

    // --- Runtime modules (simulation layer only) ---
    SignalStreamProcessor dsp;
    FeatureVectorizer vec;
    StateInferenceEngine brain;
    NeuralCalibrationEngine calib;
    ImmersionCore immersion;
    XRRenderer renderer;

    PhysiologicalGuard guard;
    EmergencyShutdown emergency;

    StabilityMonitor monitor;
    RuntimeMetrics metrics;

    XRHeadsetInterface headset({
        "wss://xr.local",
        "demo-device",
        "demo-token"
    });

    std::vector<float> fakeSignal = {0.2f, 0.4f, 0.6f, 0.3f, 0.1f};

    int cycle = 0;
    immersion.initializeBaseGameplay();

    while (running) {
        const auto loopStart = std::chrono::steady_clock::now();
        metrics.beginCycle();

        kernel.runCycle();

        // --- Simulated pipeline ---
        auto clean = dsp.denoise(fakeSignal);
        auto features = vec.vectorize(clean);
        auto state = brain.inferState(features, static_cast<double>(cycle));
        VirtualWorldState::EmbodiedInput playerInput{};
        playerInput.moveForward = (cycle % 2 == 0) ? 0.4f : 0.2f;
        playerInput.moveRight = (cycle % 3 == 0) ? 0.2f : -0.1f;
        playerInput.lookYaw = 5.0f;
        playerInput.lookPitch = 1.5f;
        playerInput.interact = ((cycle % 10) == 0);
        playerInput.vision = 0.9f;
        playerInput.audition = 0.8f;
        playerInput.touch = 0.7f;
        playerInput.smell = 0.4f;
        playerInput.taste = 0.3f;
        immersion.applyEmbodiedInput(playerInput, 0.02);
        auto calibrated = calib.calibrateForImmersion(features, 1.05f, static_cast<double>(cycle));
        auto frame = immersion.buildImmersiveFrame(calibrated);

        auto telemetry = headset.pullBioTelemetry();
        bool safe = guard.enforce(telemetry);

        // --- UI ---
        clearScreen();
        drawHeader();

        drawSection("SYSTEM");
        std::cout << "Cycle: " << cycle++ << "\n";
        std::cout << "State: " << state << "\n";
        std::cout << "Safety: " << (safe ? "OK" : "TRIGGERED") << "\n";

        drawSection("FEATURES");
        std::cout << "Mean:  " << features[0] << "\n";
        std::cout << "Min:   " << features[1] << "\n";
        std::cout << "Max:   " << features[2] << "\n";
        std::cout << "Stdev: " << features[3] << "\n";

        drawSection("IMMERSION");
        std::cout << "Frame channels: " << frame.size() << "\n";
        std::cout << "Intensity: " << calibrated[0] << "\n";
        std::cout << "Avatar pose: " << immersion.getAvatarPoseSummary() << "\n";

        drawSection("BIOMETRICS");
        if (!telemetry.empty()) {
            std::cout << "Heart rate: " << telemetry[0] << "\n";
            std::cout << "Stress:     " << telemetry[1] << "\n";
            std::cout << "Focus:      " << telemetry[2] << "\n";
        }

        drawSection("RENDER PIPELINE");
        renderer.present(frame);

        std::cout << "Frame emitted.\n";

        if (!safe) {
            drawSection("SAFETY CORE");
            std::cout << "EMERGENCY ACTIVE\n";
            emergency.trigger();
        }

        metrics.endCycle(safe, guard.isEmergencyActive(), guard.getLastEmergencyReason());

        const auto elapsedMs =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - loopStart).count();
        if (elapsedMs < kTargetLoopMs) {
            sleepMs(static_cast<int>(kTargetLoopMs - elapsedMs));
        }

        if (cycle % 20 == 0) {
            Logger::info("[RuntimeMetrics] cycles=" + std::to_string(metrics.totalCycles()) +
                         " | last_ms=" + std::to_string(metrics.lastLatencyMs()) +
                         " | avg_ms=" + std::to_string(metrics.avgLatencyMs()) +
                         " | jitter_ms=" + std::to_string(metrics.jitterMs()) +
                         " | max_ms=" + std::to_string(metrics.maxLatencyMs()) +
                         " | safety_events=" + std::to_string(metrics.safetyEventCount()));
        }
    }

    Logger::info("Shutdown complete.");
    return 0;
}
