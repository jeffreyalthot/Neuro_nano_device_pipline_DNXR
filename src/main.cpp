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

    Logger::info("Booting DNXR runtime...");

    SystemKernel kernel;
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

    XRHeadsetInterface headset({
        "wss://xr.local",
        "demo-device",
        "demo-token"
    });

    std::vector<float> fakeSignal = {0.2f, 0.4f, 0.6f, 0.3f, 0.1f};

    int cycle = 0;

    while (running) {

        kernel.runCycle();

        // --- Simulated pipeline ---
        auto clean = dsp.denoise(fakeSignal);
        auto features = vec.vectorize(clean);
        auto state = brain.inferState(features, static_cast<double>(cycle));
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

        sleepMs(250);
    }

    Logger::info("Shutdown complete.");
    return 0;
}