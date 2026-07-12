// =============================================================================
// DOSSIER : src/core/
// FICHIER : RuntimeController.cpp
//
// AMÉLIORATIONS APPLIQUÉES :
//   [3] Intégration du retour bool de endTask() : si "inference" dépasse
//       son budget et demande un abandon, le rendu est court-circuité et
//       le frame est marqué comme dégradé sans bloquer le cycle suivant.
//   [6] inducedVector tracé et audité :
//       • Sa norme L2 est calculée et comparée à kMaxInducedVectorNorm.
//       • Si elle dépasse le seuil, l'application est bloquée au démarrage.
//       • Son hash SHA-like (somme * facteur) est loggué pour traçabilité.
//       • Il est exposé dans la ConsentPolicy avec une description.
// =============================================================================

#include "RuntimeController.h"
#include "../app/CliDashboard.h"

#include "../devices/NanoNodeInterface.h"
#include "../devices/HostComputerBridge.h"
#include "../devices/XRHeadsetInterface.h"
#include "../signal/SignalStreamProcessor.h"
#include "../signal/FeatureVectorizer.h"
#include "../cognition/StateInferenceEngine.h"
#include "../calibration/NeuralCalibrationEngine.h"
#include "../calibration/CalibrationDriftMonitor.h"
#include "../immersion/ImmersionCore.h"
#include "../immersion/MultisensoryClock.h"
#include "../rendering/XRRenderer.h"
#include "../safety/PhysiologicalGuard.h"
#include "../safety/EmergencyShutdown.h"
#include "../safety/CognitiveReversibilitySystem.h"
#include "../safety/RealityAnchorSystem.h"
#include "../safety/SafetyGovernor.h"
#include "../safety/SafetyStateMachine.h"
#include "../safety/IndependentWatchdog.h"
#include "../config/SafetyProfileLoader.h"
#include "../safety/NeuralStimulusSafetyEnvelope.h"
#include "../safety/NeuroRightsPolicy.h"
#include "../signal/EnergyThermalOptimizer.h"
#include "../hardware/NeuralChipHAL.h"
#include "../neurotech/DeepImmersionPlatform.h"
#include "../security/SecretProvider.h"
#include "../runtime/FullDiveRuntimeInput.h"
#include "../runtime/FullDiveSafetyBridge.h"
#include "../runtime/RuntimeAcquisitionStage.h"
#include "../runtime/RuntimeSignalStage.h"
#include "../runtime/RuntimeInferenceStage.h"
#include "../runtime/RuntimeCalibrationStage.h"
#include "../runtime/RuntimeImmersionFrameStage.h"
#include "../runtime/RuntimeCycleFinalizer.h"
#include "../runtime/RuntimeSafetyStage.h"
#include "../runtime/RuntimeFullDiveStage.h"
#include "../runtime/RuntimeRenderStage.h"
#include "../runtime/RuntimeStimulusStage.h"
#include "../runtime/RuntimeState.h"
#include "../runtime/RuntimeFrame.h"
#include "../runtime/RuntimeTypes.h"
#include "../runtime/RuntimePipelineRunner.h"
#include "../utils/Logger.h"
#include "../utils/SessionSafetyJournal.h"
#include "../utils/TimeSync.h"
#include "../utils/AuditFileWriter.h"
#include "../performance/HotPathDiagnostics.h"
#include "../performance/PipelineBenchmark.h"
#include "../config/FullUnlockedConfig.h"

#include <algorithm>
#include <cstdlib>
#include <thread>
#include <sstream>
#include <stdexcept>
#include <cmath>
#include <numeric>
#include <utility>

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

// ─────────────────────────────────────────────────────────────────────────────
// [AMÉLIORATION 6] Validation et audit de l'inducedVector
//
// Norme L2 maximale autorisée pour le vecteur d'induction.
// Au-delà, l'influence neurale dépasse ce qui est considéré comme neutre
// et constitue un risque pour l'autonomie cognitive (neurorights).
// Valeur choisie : √(4 × 0.05²) ≈ 0.10 — permet des composantes ≤ 0.05.
// ─────────────────────────────────────────────────────────────────────────────
float computeL2Norm(const std::vector<float>& v) {
    float sum = 0.0f;
    for (float x : v) sum += x * x;
    return std::sqrt(sum);
}

// Audit léger : produit une empreinte déterministe du vecteur pour le log.
// Non cryptographique — sert uniquement à détecter un changement inattendu.
std::string auditHash(const std::vector<float>& v) {
    float acc = 0.0f;
    for (std::size_t i = 0; i < v.size(); ++i)
        acc += v[i] * static_cast<float>(i + 1) * 31.0f;
    std::ostringstream oss;
    oss << "iv_audit=" << std::hex << static_cast<int>(std::abs(acc) * 1e6f);
    return oss.str();
}


} // namespace

RuntimeController::RuntimeController()
    : RuntimeController(RuntimeConfig{}) {}

RuntimeController::RuntimeController(RuntimeConfig config)
    : config_(std::move(config)),
      runtimeMode_(runtimeModeFromString(config_.runtimeMode)),
      frameBudgetMonitor_(static_cast<double>(std::max(1, config_.targetLoopMs))),
      runtimeRunning_(false),
      frameBudgetMs_(std::max(1, config_.targetLoopMs)),
      droppedFrames_(0),
      scheduler_(20.0),
      runtimeMetrics_(static_cast<double>(frameBudgetMs_), 256),
      cycleCounter_(0),
      lastState_("init") {
    safetyJournal_ = std::make_unique<SessionSafetyJournal>();
    auditSessionPath_ = AuditFileWriter::makeTimestampedSessionPath();
    runtimeState_ = std::make_unique<RuntimeState>();
    if (config_.dashboardEnabled && !config_.headless) {
        dashboard_ = std::make_unique<CliDashboard>(config_);
    }
    pipelineRunner_ = std::make_unique<RuntimePipelineRunner>();
    initializeModules();
}

RuntimeController::~RuntimeController() {
    stop();
    shutdownModules();
}

void RuntimeController::initializeModules() {
    // Safety guardrail: this repository remains simulation-only.
    // Any attempt to switch to real hardware mode is blocked at runtime.
#if DNXR_SIMULATION_ONLY
    if (envFlagEnabled("DNXR_ALLOW_REAL_HARDWARE")) {
        throw std::runtime_error(
            "DNXR_ALLOW_REAL_HARDWARE is not permitted in this build. "
            "This project is simulation-only and not authorized for clinical/hardware use.");
    }
#endif

    nodes_ = std::make_unique<NanoNodeInterface>(NanoNodeRuntimeConfig{
        envOrDefault("DNXR_NANO_ENDPOINT",   "grpc://nano-cluster.local:7443"),
        SecretProvider::resolve("nano_auth_token", "DNXR_NANO_AUTH_TOKEN").value,
        100000U,
        true
    });

    bridge_ = std::make_unique<HostComputerBridge>(HostBridgeRuntimeConfig{
        envOrDefault("DNXR_HOST_UPSTREAM_ENDPOINT",   "grpc://host.local:9443/up"),
        envOrDefault("DNXR_HOST_DOWNSTREAM_ENDPOINT", "grpc://host.local:9443/down"),
        SecretProvider::resolve("host_auth_token", "DNXR_HOST_AUTH_TOKEN").value,
        true
    });

    headset_ = std::make_unique<XRHeadsetInterface>(XRHeadsetRuntimeConfig{
        envOrDefault("DNXR_XR_ENDPOINT", config_.xrEndpoint),
        envOrDefault("DNXR_XR_DEVICE_ID", config_.xrDeviceId),
        SecretProvider::resolve("xr_auth_token", config_.xrAuthTokenEnv).value,
        true,
        config_.xrTargetFramerate,
        120.0f,
        config_.xrAsyncTelemetry
    });

    dsp_          = std::make_unique<SignalStreamProcessor>();
    vec_          = std::make_unique<FeatureVectorizer>();
    brain_        = std::make_unique<StateInferenceEngine>();
    calib_        = std::make_unique<NeuralCalibrationEngine>();
    calibrationDriftMonitor_ = std::make_unique<CalibrationDriftMonitor>();
    immersion_    = std::make_unique<ImmersionCore>();
    multisensoryClock_ = std::make_unique<MultisensoryClock>();
    renderer_     = std::make_unique<XRRenderer>();
    guard_        = std::make_unique<PhysiologicalGuard>();
    emergency_    = std::make_unique<EmergencyShutdown>();
    reversibility_= std::make_unique<CognitiveReversibilitySystem>();
    realityAnchor_ = std::make_unique<RealityAnchorSystem>();
    thermalOptimizer_ = std::make_unique<EnergyThermalOptimizer>();
    safetyGovernor_ = std::make_unique<SafetyGovernor>();
    const SafetyRuntimeProfile safetyProfile = SafetyProfileLoader::loadFromFile();
    safetyStateMachine_ = std::make_unique<SafetyStateMachine>();
    safetyStateMachine_->bootSelfTestPassed();
    independentWatchdog_ = std::make_unique<IndependentWatchdog>(
        safetyProfile.heartbeatTimeoutMs,
        safetyProfile.latencyBudgetMs);
    independentWatchdog_->requireSubsystems({
        "runtime_cycle",
        "acquisition",
        "signal_processing",
        "inference",
        "renderer",
        "stimulus"});
    stimulusEnvelope_ = std::make_unique<NeuralStimulusSafetyEnvelope>(
        UserNeuralCalibrationProfile{
            safetyProfile.profile,
            safetyProfile.maxVisualIntensity,
            safetyProfile.maxAudioIntensity,
            safetyProfile.maxHapticIntensity,
            safetyProfile.maxChemicalIntensity,
            safetyProfile.maxBodyIntensity,
            safetyProfile.maxDeltaPerFrame,
            safetyProfile.maxVestibularIntensity,
            safetyProfile.maxProprioceptiveIntensity});
    neuralTransportSecret_ = SecretProvider::resolve("neural_transport_session", "DNXR_NEURAL_TRANSPORT_KEY");
    neuralChipHal_ = std::make_unique<SimulatedNeuralChipHAL>(64);
    neuralChipHal_->connect("runtime_simulated_neural_chip");
    deepImmersion_= std::make_unique<DeepImmersionPlatform>();
    fullDiveSynchronization_ = std::make_unique<dnxr::FullDiveSynchronizationEngine>();
    fullDiveBridge_ = std::make_unique<FullDiveSafetyBridge>();
    fullDiveSynchronization_->setUpdateMode(dnxr::FullDiveUpdateMode::RuntimeDriven);
    fullDiveSynchronization_->configureSafetyThresholds(safetyProfile);

    deepImmersion_->setConsentPolicy({true, true, false, "runtime_default"});

    // ─────────────────────────────────────────────────────────────────────
    // [AMÉLIORATION 6] Vecteur d'induction : déclaration, validation, audit
    //
    // Ce vecteur module légèrement le flux neural avant inférence.
    // Il représente un biais de calibration de départ (non nul mais infime).
    // TOUTE modification doit :
    //   1. Rester sous kMaxInducedVectorNorm (norme L2 ≤ 0.12)
    //   2. Être loggée avec son hash pour traçabilité
    //   3. Être couverte par la politique de consentement
    // ─────────────────────────────────────────────────────────────────────
    const std::vector<float> inducedVector = {0.05f, -0.02f, 0.03f, 0.01f};

    const float ivNorm = computeL2Norm(inducedVector);
    if (ivNorm > safetyProfile.maxInducedVectorNorm) {
        throw std::runtime_error(
            "inducedVector L2 norm exceeds allowed threshold ("
            + std::to_string(ivNorm) + " > "
            + std::to_string(safetyProfile.maxInducedVectorNorm)
            + "). Adjust the vector or update kMaxInducedVectorNorm "
            "with documented justification.");
    }

    const NeuroRightsDecision influenceRights = NeuroRightsPolicy::simulationDefault().evaluate(
        NeuroRightsRequest{
            NeuroPermissionScope::CognitiveInfluence,
            "RuntimeController",
            "runtime_induced_vector",
            true,
            true,
            false,
            ivNorm,
            safetyProfile.maxInducedVectorNorm});
    if (!influenceRights.allowed) {
        throw std::runtime_error("inducedVector denied by NeuroRightsPolicy: " + influenceRights.reason);
    }

    Logger::info("inducedVector validated: norm=" + std::to_string(ivNorm)
                 + " threshold=" + std::to_string(safetyProfile.maxInducedVectorNorm)
                 + " " + auditHash(inducedVector)
                 + " " + influenceRights.auditTag);

    deepImmersion_->setInducedVector(inducedVector);

    // ─────────────────────────────────────────────────────────────────────
    // [AMÉLIORATION 3] Tâches schedulées avec abortIfOverBudget activé
    // pour "inference" et "render" (les deux étapes critiques pour le
    // cybersickness). "acquisition" et "signal_processing" ne sont pas
    // en abort car une perte de frame vaut mieux qu'un abandon d'acquisition.
    // ─────────────────────────────────────────────────────────────────────
    scheduler_.registerTask({"acquisition",      100, 3.0, false});
    scheduler_.registerTask({"signal_processing", 80, 4.0, false});
    scheduler_.registerTask({"inference",          70, 5.0, true });  // abort si > 5 ms
    scheduler_.registerTask({"render",             60, 6.0, true });  // abort si > 6 ms

    const std::string sessionIdentityToken = identityVault_.tokenizeIdentity("runtime_user");
    Logger::info("identity_token_initialized=" + sessionIdentityToken);

    if (runtimeMode_ == RuntimeMode::FullUnlocked) {
        const auto unlocked = FullUnlockedConfig::defaults();
        Logger::warn("[FullUnlocked] diagnostics-only mode requested; no safety capabilities are expanded. " + unlocked.describe());
    }
    if (config_.benchmarkEnabled || runtimeMode_ == RuntimeMode::Benchmark) {
        PipelineBenchmark benchmark;
        Logger::info("[PipelineBenchmark] " + benchmark.run(config_.benchmarkCycles, [](std::uint64_t) noexcept {}));
    }

    stageServices_ = std::make_unique<RuntimeStageServices>(makeRuntimeStageServices());
    pipelineSinks_ = std::make_unique<RuntimePipelineSinks>(makeRuntimePipelineSinks());
}

void RuntimeController::shutdownModules() {
    if (safetyJournal_ && !auditSessionPath_.empty() &&
        !safetyJournal_->saveToFile(auditSessionPath_, "logs/safety/latest.audit")) {
        Logger::warn("[RuntimeController] Failed to persist safety audit journal.");
    }
    pipelineSinks_.reset();
    stageServices_.reset();
    dashboard_.reset();
    runtimeState_.reset();
    safetyJournal_.reset();
    nodes_.reset();
    bridge_.reset();
    headset_.reset();
    dsp_.reset();
    vec_.reset();
    brain_.reset();
    calib_.reset();
    calibrationDriftMonitor_.reset();
    immersion_.reset();
    multisensoryClock_.reset();
    renderer_.reset();
    guard_.reset();
    emergency_.reset();
    reversibility_.reset();
    realityAnchor_.reset();
    thermalOptimizer_.reset();
    safetyGovernor_.reset();
    safetyStateMachine_.reset();
    independentWatchdog_.reset();
    stimulusEnvelope_.reset();
    neuralChipHal_.reset();
    deepImmersion_.reset();
    fullDiveSynchronization_.reset();
    fullDiveBridge_.reset();
    pipelineRunner_.reset();
}

double RuntimeController::computeCycleDeltaSeconds(std::chrono::steady_clock::time_point cycleStart) {
    const double fallbackDeltaSeconds = static_cast<double>(std::max(1, frameBudgetMs_)) / 1000.0;
    double measuredDeltaSeconds = fallbackDeltaSeconds;
    if (lastCycleStart_ != std::chrono::steady_clock::time_point{}) {
        measuredDeltaSeconds = std::chrono::duration<double>(cycleStart - lastCycleStart_).count();
        if (measuredDeltaSeconds < 0.0) {
            measuredDeltaSeconds = 0.0;
        }
    }
    lastCycleStart_ = cycleStart;
    return measuredDeltaSeconds;
}


RuntimeAcquisitionStage::Input RuntimeController::makeAcquisitionStageInput() {
    return RuntimeAcquisitionStage::Input{
        [this]() { return headset_->pullBioTelemetry(); },
        [this](const std::vector<float>& telemetry) { return guard_->enforce(telemetry); },
        [this]() { if (independentWatchdog_) independentWatchdog_->heartbeat("runtime_cycle"); },
        [this]() { if (independentWatchdog_) independentWatchdog_->heartbeat("acquisition"); },
        [this]() { return nodes_->readNeuralNodeSignals(); },
        [this]() { return neuralChipHal_->readFrame(); }
    };
}

RuntimeSignalStage::Input RuntimeController::makeSignalStageInput() {
    return RuntimeSignalStage::Input{
        [this](const std::vector<float>& raw) { return crypto_.encrypt(raw, neuralTransportSecret_.value); },
        [this](const std::vector<std::uint8_t>& encrypted) { return crypto_.decrypt(encrypted, neuralTransportSecret_.value); },
        [this](const std::vector<float>& decrypted) { return bridge_->uplink(decrypted); },
        [this](const std::vector<float>& uplinked) { return dsp_->denoise(uplinked); },
        [this](const std::vector<float>& clean) { return vec_->vectorize(clean); },
        [this]() { if (independentWatchdog_) independentWatchdog_->heartbeat("signal_processing"); }
    };
}

RuntimeInferenceStage::Input RuntimeController::makeInferenceStageInput() {
    return RuntimeInferenceStage::Input{
        [this](const std::vector<float>& features, double ts) { return brain_->inferState(features, ts); },
        [](const std::vector<float>& telemetry) {
            BiometricFrame bio{};
            if (!telemetry.empty()) {
                bio.heartRate = telemetry[0];
                bio.galvanicSkinResponse = telemetry.size() > 1 ? telemetry[1] : 0.0f;
                bio.eyeFocus = telemetry.size() > 2 ? telemetry[2] : 0.0f;
                bio.respirationRate = 12.0f + bio.galvanicSkinResponse * 8.0f;
                bio.postureStability = telemetry.size() > 5 ? telemetry[5] : 0.75f;
                bio.movementEnergy = telemetry.size() > 6 ? telemetry[6] / 128.0f : 0.2f;
            }
            return bio;
        },
        [this](const std::vector<float>& clean, const BiometricFrame& bio) { return deepImmersion_->runInferenceTick(clean, bio, "runtime_controller"); },
        [this](const CognitiveState& cognitive, const BiometricFrame& bio) { return deepImmersion_->runAdaptationTick(cognitive, bio); },
        [this]() { return deepImmersion_->lastDiagnostics(); },
        [this]() { if (independentWatchdog_) independentWatchdog_->heartbeat("inference"); }
    };
}

void RuntimeController::publishPipelineState(const RuntimePipelineFrame& pipeline) {
    const auto& cognitive = pipeline.cognitive;
    const auto& adaptation = pipeline.adaptation;
    const auto& diag = pipeline.diagnostics;
    const std::string statePayload = pipeline.symbolicState
        + "|immersion=" + std::to_string(cognitive.immersionIndex)
        + "|stress="    + std::to_string(cognitive.stress)
        + "|shader="    + adaptation.shaderProfile
        + "|brainwave=" + diag.brainwaveClass
        + "|cue="       + diag.neurofeedbackCue
        + "|induced="   + std::to_string(diag.inducedInfluence)
        + "|drift="     + std::to_string(diag.decisionDrift)
        + "|autonomy="  + std::to_string(diag.autonomyPreservation);
    {
        std::lock_guard<std::mutex> lock(mtx_);
        lastState_ = statePayload;
    }
    publishRuntimeEvent("state:" + statePayload);
    Logger::info("🧠 State -> " + statePayload);
}


void RuntimeController::publishRuntimeArtifacts(const RuntimePipelineFrame& pipeline) {
    RuntimeMetricsSnapshot metricsSnapshot;
    {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        metricsSnapshot = runtimeMetrics_.snapshot();
    }

    RuntimeFrame runtimeFrame;
    runtimeFrame.cycleId = pipeline.cycleId;
    runtimeFrame.timestampMs = TimeSync::nowMs();
    runtimeFrame.rawSignal = pipeline.rawNeuralSignals;
    runtimeFrame.cleanSignal = pipeline.cleanSignals;
    runtimeFrame.features = pipeline.features;
    runtimeFrame.cognitiveState = pipeline.symbolicState;
    runtimeFrame.biometrics = pipeline.telemetry;
    runtimeFrame.xrAction = toString(IntentAction::Navigate);
    runtimeFrame.xrFrame = pipeline.sensoryFrame;
    runtimeFrame.safetyDecision = pipeline.safetyDecision.allowRender && !pipeline.safetyDecision.requiresEmergencyStop
        ? toString(RuntimeSafetyAction::AllowRender)
        : toString(RuntimeSafetyAction::EmergencyStop);
    runtimeFrame.metricsSnapshot = metricsSnapshot;
    if (runtimeState_) {
        runtimeState_->commitFrame(runtimeFrame);
    }

    if (dashboard_ && config_.dashboardEnabled && !config_.headless) {
        const auto renderTelemetry = renderer_ ? renderer_->getTelemetrySnapshot() : XRRenderTelemetrySnapshot{};
        CliDashboardSnapshot snapshot;
        snapshot.cycle = static_cast<int>(pipeline.cycleId);
        snapshot.cognitiveState = pipeline.symbolicState;
        snapshot.safetyOk = !pipeline.emergencyActive && pipeline.safetyDecision.allowRender;
        snapshot.features = pipeline.features;
        snapshot.frame = pipeline.sensoryFrame;
        snapshot.telemetry = pipeline.telemetry;
        snapshot.calibratedIntensity = pipeline.calibratedFeatures.empty() ? 0.0f : pipeline.calibratedFeatures[0];
        snapshot.avatarPose = immersion_ ? immersion_->getAvatarPoseSummary() : "unavailable";
        snapshot.signalSource = config_.signalSource;
        snapshot.renderQualityMode = renderTelemetry.quality.qualityMode;
        snapshot.auditHash = safetyJournal_ ? safetyJournal_->latestHash() : "genesis";
        snapshot.lastLatencyMs = metricsSnapshot.lastLatencyMs;
        snapshot.avgLatencyMs = metricsSnapshot.avgLatencyMs;
        snapshot.jitterMs = metricsSnapshot.jitterMs;
        snapshot.maxLatencyMs = metricsSnapshot.maxLatencyMs;
        snapshot.safetyEvents = metricsSnapshot.safetyEventCount;
        dashboard_->render(snapshot);
    }
}

RuntimeCalibrationStage::Input RuntimeController::makeCalibrationStageInput() {
    return RuntimeCalibrationStage::Input{
        [this](const std::vector<float>& features, float sensitivity, double ts) {
            return calib_->calibrateForImmersion(features, sensitivity, ts);
        },
        [this](const std::vector<float>& calibratedFeatures, float quality, double ts, const std::string& label) {
            return calibrationDriftMonitor_->buildBaseline(calibratedFeatures, quality, ts, label);
        },
        [this](const UserBaselineProfile& baseline) { calibrationDriftMonitor_->setBaseline(baseline); },
        [this](const std::vector<float>& calibratedFeatures, float quality, float confidence, float fatigue, double ts) {
            return calibrationDriftMonitor_->assess(calibratedFeatures, quality, confidence, fatigue, ts);
        },
        [this]() { calib_->resetStabilization(); },
        [this](std::uint64_t cycleId, const std::string& component, const std::string& event, double latencyMs, float riskScore, bool degraded, const std::string& reason) {
            std::lock_guard<std::mutex> traceLock(traceMutex_);
            traceRecorder_.record(cycleId, component, event, latencyMs, riskScore, degraded, reason);
        },
        [](const std::string& message) { Logger::warn(message); },
        1.05f,
        cycleCounter_ == 0U
    };
}

RuntimeImmersionFrameStage::Input RuntimeController::makeImmersionFrameStageInput(double timestamp) {
    return RuntimeImmersionFrameStage::Input{
        [this](const std::vector<float>& cleanSignals, const CognitiveState& cognitiveState) {
            return deepImmersion_->runFullBodyControlTick(cleanSignals, cognitiveState);
        },
        [this](const CognitiveState& cognitiveState, const XRFullBodyCommand& bodyCommand, const XRAdaptationFrame& xrAdaptation, const NeuroPipelineDiagnostics& diagnostics) {
            return immersion_->buildImmersiveFrameFromChip(cognitiveState, bodyCommand, xrAdaptation, diagnostics);
        },
        [this](const std::vector<float>& sensoryFrame, std::uint64_t cycleId, double timestampMs, double toleranceMs) {
            return multisensoryClock_->alignVector(sensoryFrame, cycleId, timestampMs, toleranceMs);
        },
        [this](std::uint64_t cycleId, const std::string& component, const std::string& event, double latencyMs, float riskScore, bool degraded, const std::string& reason) {
            std::lock_guard<std::mutex> traceLock(traceMutex_);
            traceRecorder_.record(cycleId, component, event, latencyMs, riskScore, degraded, reason);
        },
        [](const std::string& message) { Logger::warn(message); },
        timestamp * 1000.0,
        2.0
    };
}

RuntimeCycleFinalizer::Input RuntimeController::makeCycleFinalizerInput() {
    return RuntimeCycleFinalizer::Input{
        scheduler_,
        runtimeMetrics_,
        traceRecorder_,
        metricsMutex_,
        traceMutex_,
        cycleCounter_,
        droppedFrames_,
        [this](const RuntimeMetricsSnapshot& snapshot) { publishRuntimeMetrics(snapshot); },
        [](const std::string& message) { Logger::warn(message); }
    };
}

RuntimeCycleFinalizer::Context RuntimeController::makeCycleFinalizerContext(const RuntimeMetricsSnapshot& previousMetricsSnapshot) {
    return RuntimeCycleFinalizer::Context{
        scheduler_,
        runtimeMetrics_,
        traceRecorder_,
        metricsMutex_,
        traceMutex_,
        cycleCounter_,
        droppedFrames_,
        previousMetricsSnapshot
    };
}

RuntimeSafetyStage::Input RuntimeController::makeSafetyStageInput(double timestamp) {
    return RuntimeSafetyStage::Input{
        [this](const SafetyGovernorInput& input) { return safetyGovernor_->evaluate(input); },
        [this](const std::string& category, const std::string& message, float riskScore, const SessionAuditMetadata& metadata) {
            return safetyJournal_->record(category, message, riskScore, metadata);
        },
        [this](const SafetyDecision& decision) {
            return safetyStateMachine_ ? safetyStateMachine_->observeSafetyDecision(decision) : SafetyStateTransition{};
        },
        [this](const RealityAnchorContext& context, const SafetyDecision& decision) {
            return realityAnchor_->evaluate(context, decision);
        },
        [this](const RealityAnchorPlan& plan) {
            return safetyStateMachine_ ? safetyStateMachine_->observeRealityAnchorPlan(plan) : SafetyStateTransition{};
        },
        [this](const std::vector<float>& sensoryFrame, const SafetyDecision& decision) {
            return safetyGovernor_->applyDecisionToFrame(sensoryFrame, decision);
        },
        [this](const std::vector<float>& sensoryFrame, const RealityAnchorPlan& plan) {
            return realityAnchor_->applyPlanToFrame(sensoryFrame, plan);
        },
        [this](const std::vector<float>& sensoryFrame, float immersionIndex) {
            return thermalOptimizer_->optimize(sensoryFrame, immersionIndex);
        },
        [this](const std::vector<float>& riskTelemetry) {
            return reversibility_->shouldStartReturnToReality(riskTelemetry);
        },
        [this](const std::vector<float>& sensoryFrame) { return reversibility_->runDecompression(sensoryFrame); },
        [this]() { emergency_->trigger(); },
        [](const std::string& message) { Logger::warn(message); },
        [](const std::string& message) { Logger::error(message); },
        timestamp,
        emergency_->isStable()
    };
}

RuntimeFullDiveStage::Input RuntimeController::makeFullDiveStageInput(const RuntimeMetricsSnapshot& snapshot) {
    return RuntimeFullDiveStage::Input{
        [this](const FullDiveRuntimeInput& input) { fullDiveSynchronization_->updateFromRuntime(input); },
        [this]() { return fullDiveBridge_->evaluateAndRemember(*fullDiveSynchronization_); },
        [this](const FullDiveRuntimeSnapshot& snapshot, const SafetyDecision& decision) {
            return fullDiveBridge_->mergeIntoDecision(snapshot, decision);
        },
        [this](const FullDiveRuntimeSnapshot& snapshot) {
            if (safetyStateMachine_) {
                fullDiveBridge_->apply(snapshot, *safetyStateMachine_);
            }
        },
        [this](const std::string& category, const std::string& message, float riskScore, const SessionAuditMetadata& metadata) {
            return safetyJournal_->record(category, message, riskScore, metadata);
        },
        [this]() { emergency_->trigger(); },
        [this](const std::vector<float>& sensoryFrame) { return reversibility_->runDecompression(sensoryFrame); },
        snapshot
    };
}

RuntimeRenderStage::Input RuntimeController::makeRenderStageInput(
    const std::chrono::steady_clock::time_point& renderStart,
    SchedulerTickReport& tickReport) {
    return RuntimeRenderStage::Input{
        [this](const std::vector<float>& sensoryFrame) { renderer_->present(sensoryFrame); },
        [this, &renderStart, &tickReport]() { return scheduler_.endTask(RuntimeRenderStage::name(), renderStart, tickReport); },
        [this]() { ++droppedFrames_; },
        [this]() { if (independentWatchdog_) independentWatchdog_->heartbeat("renderer"); },
        [this](double latencyMs, bool neuralSignalValid, float biometricRisk, float driftScore) {
            return independentWatchdog_
                ? independentWatchdog_->assess(latencyMs, neuralSignalValid, biometricRisk, driftScore)
                : WatchdogAssessment{};
        },
        [this]() { emergency_->trigger(); },
        [this](const std::string& reason) {
            if (safetyStateMachine_) {
                safetyStateMachine_->latchEmergency(reason);
            }
        },
        [this](const std::vector<float>& sensoryFrame) { headset_->pushSensoryFrame(sensoryFrame); },
        [](const std::string& message) { Logger::warn(message); },
        [](const std::string& message) { Logger::error(message); },
        tickReport.totalMs
    };
}

RuntimeStimulusStage::Input RuntimeController::makeStimulusStageInput() {
    return RuntimeStimulusStage::Input{
        [this](std::uint64_t cycleId, const std::vector<float>& sensoryFrame, const SafetyDecision& decision, double deltaSeconds) {
            return stimulusEnvelope_->buildCommand(cycleId, sensoryFrame, decision, deltaSeconds);
        },
        [this]() { if (independentWatchdog_) independentWatchdog_->heartbeat("stimulus"); },
        [this](const SafeNeuralStimulusCommand& command) { return neuralChipHal_->writeSafeStimulus(command); },
        [this](const std::string& reason) { neuralChipHal_->emergencyStop(reason); },
        [this](const std::vector<float>& sensoryFrame) { return bridge_->downlink(sensoryFrame); },
        [this](const std::vector<float>& downlinkFrame) { nodes_->writeNeuralStimulus(downlinkFrame); }
    };
}

RuntimeStageServices RuntimeController::makeRuntimeStageServices() {
    return RuntimeStageServices{
        *nodes_,
        *bridge_,
        *headset_,
        *dsp_,
        *vec_,
        *brain_,
        *calib_,
        *calibrationDriftMonitor_,
        *immersion_,
        *multisensoryClock_,
        *renderer_,
        *guard_,
        *emergency_,
        *reversibility_,
        *realityAnchor_,
        *thermalOptimizer_,
        *safetyGovernor_,
        *safetyStateMachine_,
        *independentWatchdog_,
        *stimulusEnvelope_,
        *neuralChipHal_,
        *deepImmersion_,
        *fullDiveSynchronization_,
        *fullDiveBridge_,
        *safetyJournal_,
        crypto_,
        neuralTransportSecret_,
        identityVault_};
}

RuntimePipelineSinks RuntimeController::makeRuntimePipelineSinks() {
    RuntimePipelineSinks sinks;
    sinks.publishPipelineState = [this](const RuntimePipelineFrame& pipeline) { publishPipelineState(pipeline); };
    sinks.publishRuntimeMetrics = [this](const RuntimeMetricsSnapshot& snapshot) { publishRuntimeMetrics(snapshot); };
    sinks.recordReplayFrame = [this](const std::vector<float>& cleanSignals, double timestamp) {
        replay_.record({cleanSignals, timestamp});
    };
    sinks.triggerEmergency = [this]() { emergency_->trigger(); };
    sinks.throttleEmergency = [this]() { throttleEmergency(); };
    sinks.recordDroppedFrame = [this]() { ++droppedFrames_; };
    sinks.auditRejectedStimulus = [this](const SafeNeuralStimulusCommand& command) {
        const auto rejectionAudit = safetyJournal_->record(
            "hal_reject",
            command.safetyMode,
            command.riskScore);
        emergency_->trigger();
        Logger::error("[NeuralChipHAL] Safe stimulus command rejected before node dispatch. journal=" +
                      rejectionAudit.eventHash);
    };
    sinks.logWarn = [](const std::string& message) { Logger::warn(message); };
    sinks.logError = [](const std::string& message) { Logger::error(message); };
    return sinks;
}

RuntimePipelineFrame RuntimeController::runDeterministicCycle(double timestamp) {
    const auto cycleStart = std::chrono::steady_clock::now();
    const double runtimeDeltaSeconds = computeCycleDeltaSeconds(cycleStart);
    SchedulerTickReport tickReport = scheduler_.beginTick();

    RuntimeMetricsSnapshot fullDiveMetricsSnapshot;
    {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        fullDiveMetricsSnapshot = runtimeMetrics_.snapshot();
    }

    RuntimeStageServices& services = *stageServices_;
    const RuntimePathDecision pathDecision = runtimePathLock_.decide(
        neuralChipHal_->status(), RuntimeDeploymentMode::SimulationOnly);
    RuntimePipelineRunner::RuntimePipelineRunInputRef input{
        RuntimeStageContext{cycleCounter_.load(), timestamp, runtimeDeltaSeconds},
        tickReport,
        services,
        RuntimePipelineSinkRefs::fromSinks(*pipelineSinks_),
        fullDiveMetricsSnapshot,
        makeCycleFinalizerContext(fullDiveMetricsSnapshot)};

    RuntimePipelineFrame frame = pipelineRunner_->run(input);
    frame.runtimePathDecision = pathDecision;
    if (pathDecision.path == RuntimePathKind::PrimaryReal &&
        pathDecision.secondaryRuntimeAllowed) {
        frame.continueCycle = false;
        frame.emergencyActive = true;
        frame.stopReason = "runtime_path_lock_invalid_primary_real_state";
    }
    publishRuntimeArtifacts(frame);
    return frame;
}

void RuntimeController::runSingleCycle(double timestamp) {
    (void)runDeterministicCycle(timestamp);
}

void RuntimeController::run() {
    runtimeRunning_ = true;
    Logger::info("🎬 [Runtime] Starting execution loop...");

    using clock = std::chrono::high_resolution_clock;
    while (runtimeRunning_) {
        const auto start = clock::now();
        const double timestamp = std::chrono::duration<double>(start.time_since_epoch()).count();
        runSingleCycle(timestamp);
        const auto elapsedMs = std::chrono::duration_cast<std::chrono::milliseconds>(clock::now() - start).count();
        frameBudgetMonitor_.observe(static_cast<double>(elapsedMs));
        if (shouldStopAfterCycle()) {
            stop();
            break;
        }
        throttle(start);
    }

    emitShutdownDiagnostics();
    Logger::info("🛑 [Runtime] Execution stopped.");
}

bool RuntimeController::shouldStopAfterCycle() const {
    return config_.maxRuntimeCycles > 0 && cycleCounter_.load() >= static_cast<std::uint64_t>(config_.maxRuntimeCycles);
}

void RuntimeController::emitShutdownDiagnostics() const {
    if (isInspectionEnabled(runtimeMode_) || config_.fullUnlockedDiagnostics) {
        HotPathDiagnostics hotPath;
        Logger::info("[FrameBudget] " + frameBudgetMonitor_.report());
        Logger::info("[HotPathDiagnostics] " + hotPath.summary());
    }
}

void RuntimeController::stop() noexcept {
    if (!runtimeRunning_) return;
    runtimeRunning_ = false;
}

void RuntimeController::throttle(const std::chrono::high_resolution_clock::time_point& start) {
    using namespace std::chrono;
    auto elapsed = duration_cast<milliseconds>(high_resolution_clock::now() - start);
    auto target  = milliseconds(frameBudgetMs_);
    if (elapsed < target)
        std::this_thread::sleep_for(target - elapsed);
}

void RuntimeController::throttleEmergency() {
    std::this_thread::sleep_for(std::chrono::milliseconds(120));
}

void RuntimeController::setFrameBudgetMs(int ms) {
    frameBudgetMs_ = std::max(1, ms);
    {
        std::lock_guard<std::mutex> lock(metricsMutex_);
        runtimeMetrics_.setLatencyBudgetMs(static_cast<double>(frameBudgetMs_));
    }
    if (independentWatchdog_) {
        independentWatchdog_->setLatencyBudgetMs(static_cast<double>(frameBudgetMs_));
    }
}

int RuntimeController::getFrameBudgetMs() const {
    return frameBudgetMs_;
}

std::string RuntimeController::getLastState() const {
    std::lock_guard<std::mutex> lock(mtx_);
    return lastState_;
}

uint64_t RuntimeController::getCycleCount()    const { return cycleCounter_.load(); }
uint64_t RuntimeController::getDroppedFrames() const { return droppedFrames_.load(); }

RuntimeMetricsSnapshot RuntimeController::getRuntimeMetricsSnapshot() const {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    return runtimeMetrics_.snapshot();
}

RuntimeLatencyHealth RuntimeController::getRuntimeLatencyHealth() const {
    std::lock_guard<std::mutex> lock(metricsMutex_);
    return runtimeMetrics_.latencyHealth();
}


std::vector<RuntimeTraceEvent> RuntimeController::getRuntimeTraceEvents() const {
    std::lock_guard<std::mutex> lock(traceMutex_);
    return traceRecorder_.events();
}

std::string RuntimeController::getLatestRuntimeTraceId() const {
    std::lock_guard<std::mutex> lock(traceMutex_);
    return traceRecorder_.latestTraceId();
}

bool RuntimeController::saveRuntimeTraceJsonl(const std::string& path) const {
    std::lock_guard<std::mutex> lock(traceMutex_);
    return traceRecorder_.saveJsonl(path);
}

bool RuntimeController::shouldApplyRendererQoS(const RuntimeMetricsSnapshot& snapshot) const {
    if (snapshot.latencyHealth == RuntimeLatencyHealth::Stable) {
        return true;
    }
    if (snapshot.latencyHealth == RuntimeLatencyHealth::Fallback) {
        return true;
    }
    return snapshot.consecutiveNonStableCycles >= 2U || snapshot.windowBudgetExceededCount >= 2U;
}


void RuntimeController::publishRuntimeEvent(const std::string& payload) {
    std::function<void(const std::string&)> callback;
    {
        std::lock_guard<std::mutex> lock(mtx_);
        callback = eventCallback_;
    }
    if (callback) {
        callback(payload);
    }
}

void RuntimeController::publishRuntimeMetrics(const RuntimeMetricsSnapshot& snapshot) {
    const bool applyRendererQoS = shouldApplyRendererQoS(snapshot);
    const char* rendererQosMode = "unavailable";
    const char* rendererQualityMode = "unavailable";

    if (renderer_) {
        if (applyRendererQoS) {
            renderer_->applyRuntimeLatencyHealth(snapshot.latencyHealth);
        }
        rendererQosMode = renderer_->evaluateQoSDecision().qosMode;
        rendererQualityMode = renderer_->getAdaptiveQualityProfile().qualityMode;
    }

    publishRuntimeEvent(
        runtimeMetricsSnapshotToEventPayload(snapshot) +
        "|renderer_qos_applied=" + (applyRendererQoS ? "1" : "0") +
        "|renderer_qos=" + rendererQosMode +
        "|renderer_quality=" + rendererQualityMode);

    if (snapshot.latencyHealth != RuntimeLatencyHealth::Stable) {
        Logger::warn(
            std::string("[RuntimeMetrics] latency_health=") + runtimeLatencyHealthToString(snapshot.latencyHealth) +
            " p95_ms=" + std::to_string(snapshot.p95LatencyMs) +
            " p99_ms=" + std::to_string(snapshot.p99LatencyMs) +
            " window_budget_exceeded=" + std::to_string(snapshot.windowBudgetExceededCount) +
            " health_transitions=" + std::to_string(snapshot.latencyHealthTransitionCount) +
            " non_stable_streak=" + std::to_string(snapshot.consecutiveNonStableCycles) +
            " renderer_qos_applied=" + (applyRendererQoS ? "1" : "0") +
            " renderer_qos=" + rendererQosMode +
            " renderer_quality=" + rendererQualityMode);
    }
}

void RuntimeController::setRuntimeEventCallback(const std::function<void(const std::string&)>& cb) {
    std::lock_guard<std::mutex> lock(mtx_);
    eventCallback_ = cb;
}

} // namespace dnxr
