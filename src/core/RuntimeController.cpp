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

namespace dnxr {

void RuntimeController::runSingleCycle() {
    // Build all pipeline subsystems for this protected runtime cycle.
    NanoNodeInterface nodes;
    HostComputerBridge bridge;
    XRHeadsetInterface headset;
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
    const auto frame = immersion.buildImmersiveFrame(calibrated);

    // Deliver the resulting immersive frame and downlink command.
    renderer.present(frame);
    headset.pushSensoryFrame(frame);
    const auto down = bridge.downlink(calibrated);
    nodes.writeNeuralStimulus(down);
}

} // namespace dnxr
