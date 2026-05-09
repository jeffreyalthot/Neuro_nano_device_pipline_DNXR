#include "RuntimeController.h"
#include "../devices/NanoNodeInterface.h"
#include "../devices/HostComputerBridge.h"
#include "../devices/XRHeadsetInterface.h"
#include "../signal/SignalStreamProcessor.h"
#include "../signal/FeatureVectorizer.h"
#include "../cognition/StateInferenceEngine.h"
#include "../calibration/NeuralCalibrationEngine.h"
#include "../immersion/ImmersionCore.h"
#include "../rendering/XRRenderer.h"
#include "../safety/PhysiologicalGuard.h"
#include "../utils/Logger.h"

#include <cstdlib>
#include <string>

namespace dnxr {
namespace {

std::string envOrDefault(const char* key, const std::string& fallback) {
    // This function reads runtime environment variables while keeping deterministic defaults for deployment templates.
    if (const char* value = std::getenv(key); value != nullptr) {
        return value;
    }
    return fallback;
}

bool envFlagEnabled(const char* key) {
    if (const char* value = std::getenv(key); value != nullptr) {
        return std::string(value) == "1" || std::string(value) == "true" || std::string(value) == "TRUE";
    }
    return false;
}

} // namespace

void RuntimeController::runSingleCycle() {
    // Build all pipeline subsystems for this protected runtime cycle.
    NanoNodeInterface nodes({envOrDefault("DNXR_NANO_ENDPOINT", "grpc://nano-cluster.local:7443"),
                             envOrDefault("DNXR_NANO_AUTH_TOKEN", "replace-with-real-token"),
                             envOrDefault("DNXR_NANO_VENDOR", "generic"),
                             envOrDefault("DNXR_NANO_SIGNAL_PROTOCOL", "neural-stream-v1"),
                             100000U});
    HostComputerBridge bridge({envOrDefault("DNXR_HOST_UPSTREAM_ENDPOINT", "grpc://host.local:9443/up"),
                               envOrDefault("DNXR_HOST_DOWNSTREAM_ENDPOINT", "grpc://host.local:9443/down"),
                               envOrDefault("DNXR_HOST_AUTH_TOKEN", "replace-with-real-token")});
    XRHeadsetInterface headset({envOrDefault("DNXR_XR_ENDPOINT", "wss://xr-headset.local:8443"),
                                envOrDefault("DNXR_XR_DEVICE_ID", "replace-with-real-headset-id"),
                                envOrDefault("DNXR_XR_AUTH_TOKEN", "replace-with-real-token")});
    SignalStreamProcessor dsp;
    FeatureVectorizer vec;
    StateInferenceEngine brain;
    NeuralCalibrationEngine calib;
    ImmersionCore immersion;
    XRRenderer renderer;
    PhysiologicalGuard guard;

    // Validate wearable telemetry before any neural read/write operation.
    const auto telemetry = headset.pullBioTelemetry();
    if (!guard.enforce(telemetry)) {
        Logger::warn("Cycle aborted by physiological guard.");
        return;
    }

    // Acquire and process node signals through the host bridge.
    Logger::info("Active neural device vendor profile: " + nodes.vendorProfile());
    const auto raw = nodes.readNeuralNodeSignals();
    if (raw.empty()) {
        Logger::warn("No neural samples received from NanoNode interface.");
        return;
    }

    const auto uplinked = bridge.uplink(raw);
    const auto clean = dsp.denoise(uplinked);
    const auto features = vec.vectorize(clean);
    const auto state = brain.inferState(features);
    Logger::info("Inferred state: " + state);

    // Use bounded sensitivity to reduce instability in adaptation behavior.
    constexpr float kUserSensitivity = 1.05F;
    const auto calibrated = calib.calibrateForImmersion(features, kUserSensitivity);
    immersion.handleUserQuickMenuInput(envFlagEnabled("DNXR_UI_MENU_BUTTON_PRESS"),
                                       envFlagEnabled("DNXR_UI_MENU_QUIT_SELECTED"));
    const auto frame = immersion.buildImmersiveFrame(calibrated);

    // Deliver the resulting immersive frame and downlink command.
    renderer.renderSafetyOverlay(immersion.isQuickMenuVisible());
    renderer.present(frame);
    headset.pushSensoryFrame(frame);

    if (immersion.isSafeExitReady()) {
        Logger::info("Skipping neural stimulus write because safe exit has completed this cycle.");
        return;
    }

    const auto down = bridge.downlink(calibrated);
    nodes.writeNeuralStimulus(down);
}

} // namespace dnxr
