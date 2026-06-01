#include "RuntimeApplication.h"
#include "CliDashboard.h"
#include "RuntimeBootstrap.h"
#include "../calibration/NeuralCalibrationEngine.h"
#include "../cognition/StateInferenceEngine.h"
#include "../core/SystemKernel.h"
#include "../devices/XRHeadsetInterface.h"
#include "../immersion/ImmersionCore.h"
#include "../input/RuntimeSignalRouter.h"
#include "../rendering/XRRenderer.h"
#include "../runtime/RuntimeState.h"
#include "../runtime/RuntimeTypes.h"
#include "../safety/EmergencyShutdown.h"
#include "../safety/PhysiologicalGuard.h"
#include "../safety/StabilityMonitor.h"
#include "../signal/FeatureVectorizer.h"
#include "../signal/SignalStreamProcessor.h"
#include "../utils/Logger.h"
#include "../utils/RuntimeMetrics.h"
#include "../utils/AuditFileWriter.h"
#include "../utils/SessionSafetyJournal.h"
#include "../utils/TimeSync.h"

#include <chrono>
#include <thread>
#include <utility>

namespace dnxr {
namespace {
void sleepMs(int ms) {
    std::this_thread::sleep_for(std::chrono::milliseconds(ms));
}
} // namespace

RuntimeApplication::RuntimeApplication()
    : config_(RuntimeBootstrap::loadConfig()) {}

RuntimeApplication::RuntimeApplication(RuntimeConfig config)
    : config_(std::move(config)) {}

int RuntimeApplication::run() {
    Logger::info("Booting DNXR runtime | profile=" + config_.profile);

    SystemKernel kernel;
    kernel.setCycleDuration(config_.targetLoopMs);
    kernel.setMaxCyclesPerSecond(config_.maxCyclesPerSecond);
    kernel.boot();

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
    RuntimeState runtimeState;
    SessionSafetyJournal safetyJournal;
    const std::string auditSessionPath = AuditFileWriter::makeTimestampedSessionPath();
    RuntimeSignalRouter signalRouter(config_);
    CliDashboard dashboard(config_);

    XRHeadsetInterface headset({
        config_.xrEndpoint,
        config_.xrDeviceId,
        config_.xrAuthToken,
        config_.simulationMode,
        config_.xrTargetFramerate,
        120.0f,
        config_.xrAsyncTelemetry
    });

    int cycle = 0;
    immersion.initializeBaseGameplay();

    while (config_.maxRuntimeCycles <= 0 || cycle < config_.maxRuntimeCycles) {
        const auto loopStart = std::chrono::steady_clock::now();
        metrics.beginCycle();
        kernel.runCycle();

        const auto rawSignal = signalRouter.nextSignal(cycle);
        auto clean = dsp.denoise(rawSignal);
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
        const bool safe = guard.enforce(telemetry);
        safetyJournal.record(
            "safety_decision",
            safe ? "runtime_nominal" : guard.getLastEmergencyReason(),
            safe ? 0.0f : 1.0f,
            SessionAuditMetadata{
                static_cast<std::uint64_t>(cycle),
                safe ? toString(RuntimeSafetyAction::AllowRender) : toString(RuntimeSafetyAction::EmergencyStop),
                safe,
                !safe
            });

        renderer.present(frame);
        const auto renderTelemetry = renderer.getTelemetrySnapshot();

        CliDashboardSnapshot snapshot;
        snapshot.cycle = cycle;
        snapshot.cognitiveState = state;
        snapshot.safetyOk = safe;
        snapshot.features = features;
        snapshot.frame = frame;
        snapshot.telemetry = telemetry;
        snapshot.calibratedIntensity = calibrated.empty() ? 0.0f : calibrated[0];
        snapshot.avatarPose = immersion.getAvatarPoseSummary();
        snapshot.signalSource = signalRouter.activeSource();
        snapshot.renderQualityMode = renderTelemetry.quality.qualityMode;
        snapshot.auditHash = safetyJournal.latestHash();
        snapshot.lastLatencyMs = metrics.lastLatencyMs();
        snapshot.avgLatencyMs = metrics.avgLatencyMs();
        snapshot.jitterMs = metrics.jitterMs();
        snapshot.maxLatencyMs = metrics.maxLatencyMs();
        snapshot.safetyEvents = metrics.safetyEventCount();
        dashboard.render(snapshot);

        if (!safe) {
            Logger::warn("[RuntimeApplication] Safety core triggered emergency path.");
            emergency.trigger();
        }

        metrics.endCycle(safe, guard.isEmergencyActive(), guard.getLastEmergencyReason());

        RuntimeFrame runtimeFrame;
        runtimeFrame.cycleId = static_cast<std::uint64_t>(cycle);
        runtimeFrame.timestampMs = TimeSync::nowMs();
        runtimeFrame.rawSignal = rawSignal;
        runtimeFrame.cleanSignal = clean;
        runtimeFrame.features = features;
        runtimeFrame.cognitiveState = state;
        runtimeFrame.biometrics = telemetry;
        runtimeFrame.xrAction = playerInput.interact ? toString(IntentAction::Interact) : toString(IntentAction::Navigate);
        runtimeFrame.xrFrame = frame;
        runtimeFrame.safetyDecision = safe ? toString(RuntimeSafetyAction::AllowRender) : toString(RuntimeSafetyAction::EmergencyStop);
        runtimeFrame.metricsSnapshot = metrics.snapshot();
        runtimeState.commitFrame(runtimeFrame);

        ++cycle;

        const auto elapsedMs =
            std::chrono::duration_cast<std::chrono::milliseconds>(
                std::chrono::steady_clock::now() - loopStart).count();
        if (elapsedMs < config_.targetLoopMs) {
            sleepMs(static_cast<int>(config_.targetLoopMs - elapsedMs));
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

    if (!safetyJournal.saveToFile(auditSessionPath, "logs/safety/latest.audit")) {
        Logger::warn("[RuntimeApplication] Failed to persist safety audit journal.");
    }

    Logger::info("Shutdown complete.");
    return 0;
}

} // namespace dnxr
