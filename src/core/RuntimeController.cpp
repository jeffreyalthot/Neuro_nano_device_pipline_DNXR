#include "RuntimeController.h"
#include "../devices/NanoNodeInterface.h"
#include "../devices/HostComputerBridge.h"
#include "../devices/XRHeadsetInterface.h"
#include "../signal/SignalStreamProcessor.h"
#include "../signal/FeatureVectorizer.h"
#include "../cognition/StateInferenceEngine.h"
#include "../cognition/NeuroDigitalTwin.h"
#include "../calibration/NeuralCalibrationEngine.h"
#include "../immersion/ImmersionCore.h"
#include "../immersion/MultiSenseOrchestrator.h"
#include "../immersion/IdentityContinuityMemory.h"
#include "../rendering/XRRenderer.h"
#include "../safety/PhysiologicalGuard.h"
#include "../safety/EthicalGuardEngine.h"
#include "../safety/CognitiveReversibilitySystem.h"
#include "../utils/Logger.h"
#include "../network/OpenNeuralInteropProtocol.h"
#include "../signal/EnergyThermalOptimizer.h"

#include <cstdlib>
#include <string>
#include <vector>

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
                             envOrDefault("DNXR_NANO_ACCESS_PORTS", "usb,bluetooth,wireless,ethernet,wifi,cellular,nfc,satellite,proprietary"),
                             100000U,
                             0.35F,
                             0.08F,
                             0.65F});
    HostComputerBridge bridge({envOrDefault("DNXR_HOST_UPSTREAM_ENDPOINT", "grpc://host.local:9443/up"),
                               envOrDefault("DNXR_HOST_DOWNSTREAM_ENDPOINT", "grpc://host.local:9443/down"),
                               envOrDefault("DNXR_HOST_AUTH_TOKEN", "replace-with-real-token")});
    XRHeadsetInterface headset({envOrDefault("DNXR_XR_ENDPOINT", "wss://xr-headset.local:8443"),
                                envOrDefault("DNXR_XR_DEVICE_ID", "replace-with-real-headset-id"),
                                envOrDefault("DNXR_XR_AUTH_TOKEN", "replace-with-real-token")});
    SignalStreamProcessor dsp;
    FeatureVectorizer vec;
    StateInferenceEngine brain;
    NeuroDigitalTwin twin;
    NeuralCalibrationEngine calib;
    ImmersionCore immersion;
    MultiSenseOrchestrator multiSense;
    IdentityContinuityMemory continuityMemory;
    XRRenderer renderer;
    PhysiologicalGuard guard;
    EthicalGuardEngine ethicalGuard;
    CognitiveReversibilitySystem reversibility;
    OpenNeuralInteropProtocol interop;
    EnergyThermalOptimizer energyThermal;

    Logger::info("Runtime cycle started: sensing -> inference -> calibration -> immersion -> actuation.");

    // Validate wearable telemetry before any neural read/write operation.
    const auto telemetry = headset.pullBioTelemetry();
    Logger::info("Bio telemetry samples pulled: " + std::to_string(telemetry.size()));
    if (!guard.enforce(telemetry) || !ethicalGuard.enforceNonBypassableLimits(telemetry, "baseline")) {
        Logger::warn("Cycle aborted by physiological guard.");
        return;
    }

    // Acquire and process node signals through the host bridge.
    Logger::info("Active neural device vendor profile: " + nodes.vendorProfile());
    Logger::info("Active neural device access ports: " + nodes.accessPortProfile());
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
    const TwinState twinState = twin.update(state, telemetry);
    Logger::info("Neuro-digital twin updated (load=" + std::to_string(twinState.cognitiveLoad) + ", stress=" + std::to_string(twinState.physiologicalStress) + ").");

    // Use bounded sensitivity to reduce instability in adaptation behavior.
    constexpr float kUserSensitivity = 1.05F;
    const auto calibrated = calib.calibrateForImmersion(features, kUserSensitivity);
    immersion.updateUserCognitiveState(state);
    immersion.handleUserQuickMenuInput(envFlagEnabled("DNXR_UI_MENU_BUTTON_PRESS"),
                                       envFlagEnabled("DNXR_UI_MENU_QUIT_SELECTED"));
    const auto frame = immersion.buildImmersiveFrame(calibrated);
    continuityMemory.save(immersion.currentWorldState().sceneName, immersion.comfortGate(), state);
    const auto continuity = continuityMemory.restore();
    Logger::info("Continuity memory anchor restored: " + continuity.spatialAnchor + "/" + continuity.narrativeAnchor);

    // Build neural communication vectors for spatial audio using comfort-gated calibration.
    std::vector<float> audioVector;
    if (frame.size() > 3) {
        audioVector.assign(frame.begin() + 3, frame.end());
    }
    const auto calibratedAudioVector = calib.calibrateSpatialAudioVectors(audioVector, kUserSensitivity, immersion.comfortGate());

    // Deliver the resulting immersive frame and downlink command.
    renderer.renderSafetyOverlay(immersion.isQuickMenuVisible());
    auto orchestratedFrame = multiSense.orchestrate(frame, twin.recommendedIntensity(), !ethicalGuard.enforceNonBypassableLimits(telemetry, state));
    if (reversibility.shouldStartReturnToReality(telemetry)) {
        Logger::warn("Cognitive auto-reversibility triggered: progressive return to reality started.");
        orchestratedFrame = reversibility.runDecompression(orchestratedFrame);
    }
    renderer.present(orchestratedFrame, immersion.currentWorldState());
    headset.pushSensoryFrame(orchestratedFrame);

    if (immersion.isSafeExitReady()) {
        Logger::info("Skipping neural stimulus write because safe exit has completed this cycle.");
        return;
    }

    auto down = bridge.downlink(calibrated);
    down.insert(down.end(), calibratedAudioVector.begin(), calibratedAudioVector.end());
    down = energyThermal.optimize(down, continuity.presenceLevel);
    down = interop.encodeStandardPacket(down, nodes.vendorProfile());
    Logger::info("Open interop metadata: " + interop.metadataTemplate());
    Logger::info("Ethical audit: " + ethicalGuard.signedAuditRecord("stimulus_write", down));
    nodes.writeNeuralStimulus(down);
}

} // namespace dnxr
