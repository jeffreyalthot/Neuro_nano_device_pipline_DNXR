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
#include "../safety/EmergencyShutdown.h"
#include "../neurotech/DeepImmersionPlatform.h"
#include "../utils/Logger.h"

#include <cstdlib>
#include <thread>
#include <sstream>
#include <stdexcept>

namespace dnxr {

namespace {
std::string envOrDefault(const char* key, const std::string& fallback) {
    if (const char* v = std::getenv(key))
        return v;
    return fallback;
}

bool envFlagEnabled(const char* key) {
    if (const char* v = std::getenv(key)) {
        return std::string(v) == "1" || std::string(v) == "true" || std::string(v) == "TRUE";
    }
    return false;
}
}

RuntimeController::RuntimeController()
    : runtimeRunning_(false),
      frameBudgetMs_(11),   // ~90 Hz
      droppedFrames_(0),
      scheduler_(20.0),
      cycleCounter_(0),
      lastState_("init") {
    initializeModules();
}

RuntimeController::~RuntimeController() {
    stop();
    shutdownModules();
}

void RuntimeController::initializeModules() {
    // Safety guardrail: this repository remains simulation-only.
    // Any attempt to switch to real hardware mode is blocked at runtime.
    if (envFlagEnabled("DNXR_ALLOW_REAL_HARDWARE")) {
        throw std::runtime_error(
            "DNXR_ALLOW_REAL_HARDWARE is not permitted in this build. "
            "This project is simulation-only and not authorized for clinical/hardware use.");
    }

    nodes_ = std::make_unique<NanoNodeInterface>(NanoNodeRuntimeConfig{
        envOrDefault("DNXR_NANO_ENDPOINT", "grpc://nano-cluster.local:7443"),
        envOrDefault("DNXR_NANO_AUTH_TOKEN", "token-placeholder"),
        100000U,
        true
    });

    bridge_ = std::make_unique<HostComputerBridge>(HostBridgeRuntimeConfig{
        envOrDefault("DNXR_HOST_UPSTREAM_ENDPOINT", "grpc://host.local:9443/up"),
        envOrDefault("DNXR_HOST_DOWNSTREAM_ENDPOINT", "grpc://host.local:9443/down"),
        envOrDefault("DNXR_HOST_AUTH_TOKEN", "token-placeholder"),
        true
    });

    headset_ = std::make_unique<XRHeadsetInterface>(XRHeadsetRuntimeConfig{
        envOrDefault("DNXR_XR_ENDPOINT", "wss://xr-headset.local:8443"),
        envOrDefault("DNXR_XR_DEVICE_ID", "headset-placeholder"),
        envOrDefault("DNXR_XR_AUTH_TOKEN", "token-placeholder"),
        true
    });

    dsp_ = std::make_unique<SignalStreamProcessor>();
    vec_ = std::make_unique<FeatureVectorizer>();
    brain_ = std::make_unique<StateInferenceEngine>();
    calib_ = std::make_unique<NeuralCalibrationEngine>();
    immersion_ = std::make_unique<ImmersionCore>();
    renderer_ = std::make_unique<XRRenderer>();
    guard_ = std::make_unique<PhysiologicalGuard>();
    emergency_ = std::make_unique<EmergencyShutdown>();
    deepImmersion_ = std::make_unique<DeepImmersionPlatform>();
    deepImmersion_->setConsentPolicy({true, true, false, "runtime_default"});
    deepImmersion_->setInducedVector({0.05f, -0.02f, 0.03f, 0.01f}); // runtime_induced_vector

    scheduler_.registerTask({"acquisition", 100, 3.0});
    scheduler_.registerTask({"signal_processing", 80, 4.0});
    scheduler_.registerTask({"inference", 70, 5.0});
    scheduler_.registerTask({"render", 60, 6.0});

    const std::string sessionIdentityToken = identityVault_.tokenizeIdentity("runtime_user");
    Logger::info("identity_token_initialized=" + sessionIdentityToken);
}

void RuntimeController::shutdownModules() {
    nodes_.reset();
    bridge_.reset();
    headset_.reset();
    dsp_.reset();
    vec_.reset();
    brain_.reset();
    calib_.reset();
    immersion_.reset();
    renderer_.reset();
    guard_.reset();
    emergency_.reset();
    deepImmersion_.reset();
}

void RuntimeController::runSingleCycle(double timestamp) {
    SchedulerTickReport tickReport = scheduler_.beginTick();
    // Lecture biométrique
    const auto tAcq = std::chrono::steady_clock::now();
    const auto telemetry = headset_->pullBioTelemetry();
    if (!guard_->enforce(telemetry)) {
        emergency_->trigger();
        Logger::warn("⚠️  Physiological guard triggered.");
        throttleEmergency();
        return;
    }

    // Lecture signaux neuronaux
    auto raw = nodes_->readNeuralNodeSignals();
    scheduler_.endTask("acquisition", tAcq, tickReport);
    if (raw.empty()) {
        Logger::warn("⚠️  Empty neural signal frame.");
        return;
    }

    // Traitement pipeline
    const auto tSignal = std::chrono::steady_clock::now();
    const std::vector<std::uint8_t> encrypted = crypto_.encrypt(raw, "dnxr_key_2026_secure");
    const auto decrypted = crypto_.decrypt(encrypted, "dnxr_key_2026_secure");
    auto uplinked = bridge_->uplink(decrypted.empty() ? raw : decrypted);
    auto clean = dsp_->denoise(uplinked);
    auto features = vec_->vectorize(clean);

    auto state = brain_->inferState(features, timestamp);
    scheduler_.endTask("signal_processing", tSignal, tickReport);

    BiometricFrame bio{};
    if (!telemetry.empty()) {
        bio.heartRate = telemetry[0];
        bio.galvanicSkinResponse = telemetry.size() > 1 ? telemetry[1] : 0.0f;
        bio.eyeFocus = telemetry.size() > 2 ? telemetry[2] : 0.0f;
        bio.respirationRate = 12.0f + bio.galvanicSkinResponse * 8.0f;
        bio.postureStability = telemetry.size() > 5 ? telemetry[5] : 0.75f;
        bio.movementEnergy = telemetry.size() > 6 ? telemetry[6] / 128.0f : 0.2f;
    }

    const auto tInference = std::chrono::steady_clock::now();
    const CognitiveState cognitive = deepImmersion_->runInferenceTick(clean, bio, "runtime_controller");
    scheduler_.endTask("inference", tInference, tickReport);
    const XRAdaptationFrame adaptation = deepImmersion_->runAdaptationTick(cognitive, bio);
    const NeuroPipelineDiagnostics diag = deepImmersion_->lastDiagnostics();

    lastState_ = state + "|immersion=" + std::to_string(cognitive.immersionIndex)
        + "|stress=" + std::to_string(cognitive.stress)
        + "|shader=" + adaptation.shaderProfile
        + "|brainwave=" + diag.brainwaveClass
        + "|cue=" + diag.neurofeedbackCue
        + "|induced=" + std::to_string(diag.inducedInfluence)
        + "|drift=" + std::to_string(diag.decisionDrift)
        + "|autonomy=" + std::to_string(diag.autonomyPreservation);

    if (eventCallback_)
        eventCallback_("state:" + lastState_);

    Logger::info("🧠 State -> " + lastState_);

    constexpr float kUserSensitivity = 1.05f;
    auto calibrated = calib_->calibrateForImmersion(features, kUserSensitivity, timestamp);

    auto frame = immersion_->buildImmersiveFrame(calibrated);
    if (!frame.empty()) {
        frame[0] *= (0.70f + adaptation.sceneIntensity * 0.60f);
        if (frame.size() > 1) {
            frame[1] *= (0.75f + adaptation.musicArousal * 0.50f);
        }
    }

    replay_.record({clean, timestamp});
    const auto tRender = std::chrono::steady_clock::now();
    renderer_->present(frame);
    scheduler_.endTask("render", tRender, tickReport);

    headset_->pushSensoryFrame(frame);

    auto down = bridge_->downlink(calibrated);
    nodes_->writeNeuralStimulus(down);

    scheduler_.endTick(tickReport);
    if (!tickReport.withinBudget) {
        ++droppedFrames_;
        Logger::warn("⚠️ Runtime frame budget exceeded total_ms=" + std::to_string(tickReport.totalMs) + " jitter_ms=" + std::to_string(tickReport.jitterMs));
    }

    ++cycleCounter_;
}

void RuntimeController::run() {
    runtimeRunning_ = true;
    Logger::info("🎬 [Runtime] Starting execution loop...");

    using clock = std::chrono::high_resolution_clock;
    while (runtimeRunning_) {
        const auto start = clock::now();
        const double timestamp = std::chrono::duration<double>(start.time_since_epoch()).count();
        runSingleCycle(timestamp);
        throttle(start);
    }

    Logger::info("🛑 [Runtime] Execution stopped.");
}

void RuntimeController::stop() noexcept {
    if (!runtimeRunning_) return;
    runtimeRunning_ = false;
}

void RuntimeController::throttle(const std::chrono::high_resolution_clock::time_point& start) {
    using namespace std::chrono;

    auto elapsed = duration_cast<milliseconds>(high_resolution_clock::now() - start);
    auto target = milliseconds(frameBudgetMs_);

    if (elapsed < target)
        std::this_thread::sleep_for(target - elapsed);
}

void RuntimeController::throttleEmergency() {
    std::this_thread::sleep_for(std::chrono::milliseconds(120));
}

void RuntimeController::setFrameBudgetMs(int ms) {
    frameBudgetMs_ = std::max(1, ms);
}

int RuntimeController::getFrameBudgetMs() const {
    return frameBudgetMs_;
}

std::string RuntimeController::getLastState() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return lastState_;
}

uint64_t RuntimeController::getCycleCount() const { return cycleCounter_; }
uint64_t RuntimeController::getDroppedFrames() const { return droppedFrames_; }

void RuntimeController::setRuntimeEventCallback(const std::function<void(const std::string&)>& cb) {
    eventCallback_ = cb;
}

} // namespace dnxr
