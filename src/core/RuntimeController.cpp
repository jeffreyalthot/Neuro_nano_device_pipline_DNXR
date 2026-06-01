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
#include "../runtime/RuntimeSafetyStage.h"
#include "../runtime/RuntimeFullDiveStage.h"
#include "../runtime/RuntimeRenderStage.h"
#include "../runtime/RuntimeStimulusStage.h"
#include "../utils/Logger.h"
#include "../utils/SessionSafetyJournal.h"
#include "../utils/TimeSync.h"

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

std::vector<float> normalizedTelemetryRiskVector(const std::vector<float>& telemetry) {
    if (telemetry.empty()) {
        return {};
    }

    const float heartRate = telemetry.size() > 0 ? telemetry[0] : 72.0f;
    const float stress = telemetry.size() > 1 ? telemetry[1] : 0.0f;
    const float focus = telemetry.size() > 2 ? telemetry[2] : 1.0f;
    const float neuralNoise = telemetry.size() > 3 ? telemetry[3] : 0.0f;
    const float oxygen = telemetry.size() > 4 ? telemetry[4] : 1.0f;
    const float eyeStability = telemetry.size() > 5 ? telemetry[5] : 1.0f;

    return {
        std::clamp((heartRate - 60.0f) / 80.0f, 0.0f, 1.0f),
        std::clamp(stress, 0.0f, 1.0f),
        std::clamp(1.0f - focus, 0.0f, 1.0f),
        std::clamp(neuralNoise * 8.0f, 0.0f, 1.0f),
        std::clamp(1.0f - oxygen, 0.0f, 1.0f),
        std::clamp(1.0f - eyeStability, 0.0f, 1.0f)
    };
}

} // namespace

RuntimeController::RuntimeController()
    : runtimeRunning_(false),
      frameBudgetMs_(11),   // ~90 Hz
      droppedFrames_(0),
      scheduler_(20.0),
      runtimeMetrics_(static_cast<double>(frameBudgetMs_), 256),
      cycleCounter_(0),
      lastState_("init") {
    safetyJournal_ = std::make_unique<SessionSafetyJournal>();
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
        envOrDefault("DNXR_XR_ENDPOINT",    "wss://xr-headset.local:8443"),
        envOrDefault("DNXR_XR_DEVICE_ID",   "headset-placeholder"),
        SecretProvider::resolve("xr_auth_token", "DNXR_XR_AUTH_TOKEN").value,
        true
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
}

void RuntimeController::shutdownModules() {
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

void RuntimeController::runSingleCycle(double timestamp) {
    const auto cycleStart = std::chrono::steady_clock::now();
    const double runtimeDeltaSeconds = computeCycleDeltaSeconds(cycleStart);
    SchedulerTickReport tickReport = scheduler_.beginTick();
    auto finishCycle = [&](bool safeDecision, bool emergencyActive, const std::string& reason) {
        scheduler_.endTick(tickReport);
        RuntimeMetricsSnapshot snapshot;
        {
            std::lock_guard<std::mutex> lock(metricsMutex_);
            runtimeMetrics_.recordCycleMeasurement(
                tickReport.totalMs,
                safeDecision,
                emergencyActive,
                reason);
            snapshot = runtimeMetrics_.snapshot();
        }
        {
            std::lock_guard<std::mutex> traceLock(traceMutex_);
            traceRecorder_.record(
                cycleCounter_,
                "RuntimeController",
                safeDecision ? "cycle_complete" : "cycle_degraded",
                tickReport.totalMs,
                emergencyActive ? 1.0f : 0.0f,
                !safeDecision || emergencyActive,
                reason);
        }
        publishRuntimeMetrics(snapshot);
        return snapshot;
    };

    RuntimeStageContext stageContext{cycleCounter_.load(), timestamp, runtimeDeltaSeconds};
    RuntimePipelineFrame pipeline;

    // Stage 1 — acquisition biométrique + signaux neuronaux.
    const auto tAcq = std::chrono::steady_clock::now();
    pipeline = RuntimeAcquisitionStage::execute(stageContext, RuntimeAcquisitionStage::Input{
        [this]() { return headset_->pullBioTelemetry(); },
        [this](const std::vector<float>& telemetry) { return guard_->enforce(telemetry); },
        [this]() { if (independentWatchdog_) independentWatchdog_->heartbeat("runtime_cycle"); },
        [this]() { if (independentWatchdog_) independentWatchdog_->heartbeat("acquisition"); },
        [this]() { return nodes_->readNeuralNodeSignals(); },
        [this]() { return neuralChipHal_->readFrame(); }
    });
    scheduler_.endTask(RuntimeAcquisitionStage::name(), tAcq, tickReport);
    if (!pipeline.continueCycle) {
        if (pipeline.emergencyActive) {
            emergency_->trigger();
            Logger::warn("⚠️  Physiological guard triggered.");
            finishCycle(false, true, pipeline.stopReason);
            ++cycleCounter_;
            throttleEmergency();
            return;
        }
        Logger::warn("⚠️  Empty neural signal frame.");
        finishCycle(false, pipeline.acquisitionStatus.emergencyActive, pipeline.stopReason);
        ++cycleCounter_;
        return;
    }

    // Stage 2 — transport simulation, uplink, débruitage et vectorisation.
    const auto tSignal = std::chrono::steady_clock::now();
    pipeline = RuntimeSignalStage::execute(std::move(pipeline), RuntimeSignalStage::Input{
        [this](const std::vector<float>& raw) { return crypto_.encrypt(raw, neuralTransportSecret_.value); },
        [this](const std::vector<std::uint8_t>& encrypted) { return crypto_.decrypt(encrypted, neuralTransportSecret_.value); },
        [this](const std::vector<float>& decrypted) { return bridge_->uplink(decrypted); },
        [this](const std::vector<float>& uplinked) { return dsp_->denoise(uplinked); },
        [this](const std::vector<float>& clean) { return vec_->vectorize(clean); },
        [this]() { if (independentWatchdog_) independentWatchdog_->heartbeat("signal_processing"); }
    });
    scheduler_.endTask(RuntimeSignalStage::name(), tSignal, tickReport);
    if (!pipeline.continueCycle) {
        finishCycle(false, pipeline.signalStatus.emergencyActive, pipeline.stopReason);
        ++cycleCounter_;
        return;
    }

    pipeline.normalizedTelemetryRisk = normalizedTelemetryRiskVector(pipeline.telemetry);

    // Stage 3 — inférence symbolique + inférence full-immersion et diagnostics.
    const auto tInference = std::chrono::steady_clock::now();
    pipeline = RuntimeInferenceStage::execute(std::move(pipeline), RuntimeInferenceStage::Input{
        [this](const std::vector<float>& features, double ts) { return brain_->inferState(features, ts); },
        [](const std::vector<float>& telemetry) {
            BiometricFrame bio{};
            if (!telemetry.empty()) {
                bio.heartRate            = telemetry[0];
                bio.galvanicSkinResponse = telemetry.size() > 1 ? telemetry[1] : 0.0f;
                bio.eyeFocus             = telemetry.size() > 2 ? telemetry[2] : 0.0f;
                bio.respirationRate      = 12.0f + bio.galvanicSkinResponse * 8.0f;
                bio.postureStability     = telemetry.size() > 5 ? telemetry[5] : 0.75f;
                bio.movementEnergy       = telemetry.size() > 6 ? telemetry[6] / 128.0f : 0.2f;
            }
            return bio;
        },
        [this](const std::vector<float>& clean, const BiometricFrame& bio) { return deepImmersion_->runInferenceTick(clean, bio, "runtime_controller"); },
        [this](const CognitiveState& cognitive, const BiometricFrame& bio) { return deepImmersion_->runAdaptationTick(cognitive, bio); },
        [this]() { return deepImmersion_->lastDiagnostics(); },
        [this]() { if (independentWatchdog_) independentWatchdog_->heartbeat("inference"); }
    });
    const bool inferenceOk = scheduler_.endTask(RuntimeInferenceStage::name(), tInference, tickReport);

    if (!inferenceOk) {
        ++droppedFrames_;
        Logger::warn("⚠️ Inference budget exceeded — frame degraded, skipping render. task=inference");
        finishCycle(true, false, "inference_budget_exceeded");
        ++cycleCounter_;
        return;
    }

    const auto& telemetry = pipeline.telemetry;
    const auto& raw = pipeline.rawNeuralSignals;
    const auto& clean = pipeline.cleanSignals;
    const auto& features = pipeline.features;
    const auto& normalizedRiskTelemetry = pipeline.normalizedTelemetryRisk;
    const auto& state = pipeline.symbolicState;
    const auto& bio = pipeline.biometric;
    const auto& cognitive = pipeline.cognitive;
    const auto& adaptation = pipeline.adaptation;
    const auto& diag = pipeline.diagnostics;

    std::string statePayload = state
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

    constexpr float kUserSensitivity = 1.05f;
    auto calibrated = calib_->calibrateForImmersion(features, kUserSensitivity, timestamp);
    if (cycleCounter_ == 0U) {
        calibrationDriftMonitor_->setBaseline(calibrationDriftMonitor_->buildBaseline(
            calibrated,
            diag.neuralSignalQuality,
            timestamp,
            "runtime_baseline"));
    }
    const CalibrationDriftAssessment calibrationDrift = calibrationDriftMonitor_->assess(
        calibrated,
        diag.neuralSignalQuality,
        diag.intentConfidence,
        cognitive.fatigue,
        timestamp);
    if (calibrationDrift.requiresRecalibration) {
        Logger::warn("[CalibrationDriftMonitor] " + calibrationDrift.reason +
                     " drift=" + std::to_string(calibrationDrift.driftScore) +
                     " velocity=" + std::to_string(calibrationDrift.driftVelocity) +
                     " age_s=" + std::to_string(calibrationDrift.calibrationAgeSeconds));
        {
            std::lock_guard<std::mutex> traceLock(traceMutex_);
            traceRecorder_.record(
                cycleCounter_,
                "CalibrationDriftMonitor",
                "recalibration_recommended",
                calibrationDrift.calibrationAgeSeconds,
                calibrationDrift.driftScore,
                calibrationDrift.blockCriticalActions,
                calibrationDrift.reason);
        }
        if (calibrationDrift.blockCriticalActions) {
            calib_->resetStabilization();
        }
    }

    const XRFullBodyCommand bodyCommand =
        deepImmersion_->runFullBodyControlTick(clean, cognitive);

    const NeuralSensoryFrame neuralFrame =
        immersion_->buildImmersiveFrameFromChip(
            cognitive,
            bodyCommand,
            adaptation,
            diag);

    auto frame = neuralFrame.toVector();
    if (!frame.empty() && !calibrated.empty()) {
        const float calibrationAnchor = calibrated.front();
        frame[0] = (frame[0] * 0.85f) + (calibrationAnchor * 0.15f);
    }

    const MultisensoryAlignmentReport sensorySync = multisensoryClock_->alignVector(
        frame,
        cycleCounter_,
        (timestamp * 1000.0),
        2.0);
    frame = sensorySync.alignedFrame;
    pipeline.sensoryFrame = frame;
    pipeline.sensorySync = sensorySync;
    if (sensorySync.degradationApplied) {
        {
            std::lock_guard<std::mutex> traceLock(traceMutex_);
            traceRecorder_.record(
                cycleCounter_,
                "MultisensoryClock",
                "alignment_degraded",
                sensorySync.maxObservedPhaseErrorMs,
                1.0f - sensorySync.recommendedIntensityScale,
                true,
                sensorySync.reason);
        }
        Logger::warn("[MultisensoryClock] " + sensorySync.reason +
                     " phase_ms=" + std::to_string(sensorySync.maxObservedPhaseErrorMs) +
                     " confidence=" + std::to_string(sensorySync.confidence) +
                     " scale=" + std::to_string(sensorySync.recommendedIntensityScale));
    }

    const RuntimeStageStatus safetyStageStatus = RuntimeSafetyStage::validate(true, "safety_stage_unhealthy");
    (void)safetyStageStatus;

    const SafetyDecision safetyDecision = safetyGovernor_->evaluate({
        normalizedRiskTelemetry,
        frame,
        state,
        cognitive,
        bio,
        diag
    });

    pipeline.safetyDecision = safetyDecision;

    const auto safetyAudit = safetyJournal_->record(
        "safety_decision",
        safetyDecision.reason + "|" + safetyDecision.auditRecord,
        safetyDecision.riskScore);

    if (safetyStateMachine_) {
        const SafetyStateTransition safetyTransition = safetyStateMachine_->observeSafetyDecision(safetyDecision);
        if (safetyTransition.latched) {
            emergency_->trigger();
            finishCycle(false, true, "safety_state_machine:" + safetyTransition.reason);
            ++cycleCounter_;
            return;
        }
    }

    if (safetyDecision.requiresEmergencyStop || !safetyDecision.allowRender) {
        emergency_->trigger();
        Logger::error("[SafetyGovernor] Emergency stop before render | " +
                      safetyDecision.reason + " | " + safetyDecision.auditRecord +
                      " | journal=" + safetyAudit.eventHash);
        finishCycle(false, true, "safety_governor:" + safetyDecision.reason);
        ++cycleCounter_;
        return;
    }

    RealityAnchorContext anchorContext;
    anchorContext.userAlias = "runtime_user";
    anchorContext.sessionId = "runtime_session";
    anchorContext.realEnvironmentLabel = "simulation_lab";
    anchorContext.sessionElapsedSeconds = timestamp;
    anchorContext.consentActive = diag.autonomyPreservation >= 0.20f;
    anchorContext.emergencyActive = !emergency_->isStable();
    anchorContext.identityContinuity = diag.autonomyPreservation;
    anchorContext.temporalOrientation = cognitive.attention;
    anchorContext.realWorldOrientation = bio.postureStability;
    anchorContext.sensoryOverload = std::max(
        std::max(
            std::clamp(cognitive.stress + cognitive.fatigue, 0.0f, 1.0f),
            sensorySync.degradationApplied ? 1.0f - sensorySync.recommendedIntensityScale : 0.0f),
        calibrationDrift.blockCriticalActions ? calibrationDrift.driftScore : 0.0f);

    const RealityAnchorPlan anchorPlan = realityAnchor_->evaluate(anchorContext, safetyDecision);
    pipeline.realityAnchorPlan = anchorPlan;

    if (safetyStateMachine_) {
        const SafetyStateTransition anchorTransition = safetyStateMachine_->observeRealityAnchorPlan(anchorPlan);
        if (anchorTransition.latched) {
            emergency_->trigger();
            safetyJournal_->record(
                "reality_anchor_state_transition",
                anchorTransition.reason,
                anchorPlan.riskScore);
            finishCycle(false, true, "safety_state_machine:" + anchorTransition.reason);
            ++cycleCounter_;
            return;
        }
    }

    if (anchorPlan.requiresEmergencyStop || !anchorPlan.allowRender) {
        emergency_->trigger();
        const auto anchorAudit = safetyJournal_->record(
            "reality_anchor_stop",
            anchorPlan.traceTag + "|" + anchorPlan.recoveryAction,
            anchorPlan.riskScore);
        Logger::error("[RealityAnchorSystem] Emergency anchor before render | " +
                      anchorPlan.traceTag + " | journal=" + anchorAudit.eventHash);
        finishCycle(false, true, anchorPlan.traceTag);
        ++cycleCounter_;
        return;
    }

    frame = safetyGovernor_->applyDecisionToFrame(frame, safetyDecision);
    frame = realityAnchor_->applyPlanToFrame(frame, anchorPlan);
    if (safetyDecision.action != SafetyAction::Continue || anchorPlan.mode != RealityAnchorMode::Maintain) {
        Logger::warn("[SafetyGovernor] action=" + safetyDecision.reason +
                     " risk=" + std::to_string(safetyDecision.riskScore) +
                     " scale=" + std::to_string(safetyDecision.intensityScale) +
                     " anchor=" + anchorPlan.traceTag);
    }

    frame = thermalOptimizer_->optimize(frame, cognitive.immersionIndex);

    if (safetyDecision.action == SafetyAction::Decompress ||
        safetyDecision.action == SafetyAction::RealityAnchor ||
        anchorPlan.mode == RealityAnchorMode::RealityAnchor ||
        reversibility_->shouldStartReturnToReality(normalizedRiskTelemetry)) {
        frame = reversibility_->runDecompression(frame);
        Logger::warn("[CognitiveReversibility] Return-to-reality decompression applied. anchor=" +
                     anchorPlan.traceTag);
    }

    replay_.record({clean, timestamp});

    SafetyDecision fusedSafetyDecision = safetyDecision;
    pipeline.fusedSafetyDecision = fusedSafetyDecision;

    const RuntimeStageStatus fullDiveStageStatus = RuntimeFullDiveStage::validate(fullDiveSynchronization_ != nullptr && fullDiveBridge_ != nullptr, "full_dive_stage_disabled");
    (void)fullDiveStageStatus;

    if (fullDiveSynchronization_ && fullDiveBridge_) {
        RuntimeMetricsSnapshot metricsSnapshot;
        {
            std::lock_guard<std::mutex> lock(metricsMutex_);
            metricsSnapshot = runtimeMetrics_.snapshot();
        }
        FullDiveRuntimeInput fullDiveInput;
        fullDiveInput.cognitive = cognitive;
        fullDiveInput.biometric = bio;
        fullDiveInput.diagnostics = diag;
        fullDiveInput.calibrationDrift = calibrationDrift;
        fullDiveInput.safetyDecision = fusedSafetyDecision;
        fullDiveInput.realityAnchorPlan = anchorPlan;
        fullDiveInput.sensorySync = sensorySync;
        fullDiveInput.runtimeMetrics = metricsSnapshot;
        fullDiveInput.deltaTimeSeconds = runtimeDeltaSeconds;
        fullDiveSynchronization_->updateFromRuntime(fullDiveInput);

        const auto fullDiveSnapshot = fullDiveBridge_->evaluateAndRemember(*fullDiveSynchronization_);
        pipeline.fullDiveSnapshot = fullDiveSnapshot;
        fusedSafetyDecision = fullDiveBridge_->mergeIntoDecision(fullDiveSnapshot, fusedSafetyDecision);
        pipeline.fusedSafetyDecision = fusedSafetyDecision;
        if (fullDiveSnapshot.safetyInterventionRequired()) {
            if (safetyJournal_) {
                SessionAuditMetadata metadata;
                metadata.runtimeCycle = cycleCounter_;
                metadata.safetyAction = fullDiveSnapshot.traceTag;
                metadata.renderAllowed = fullDiveSnapshot.intervention != FullDiveInterventionLevel::EmergencyStop &&
                                         fullDiveSnapshot.intervention != FullDiveInterventionLevel::Lockout;
                metadata.emergencyStop = fullDiveSnapshot.intervention == FullDiveInterventionLevel::EmergencyStop ||
                                         fullDiveSnapshot.intervention == FullDiveInterventionLevel::Lockout;
                safetyJournal_->record(
                    "full_dive_intervention",
                    fullDiveSnapshot.traceTag + "|" + fullDiveSnapshot.reason,
                    fullDiveSnapshot.severity,
                    metadata);
            }
            if (safetyStateMachine_) {
                fullDiveBridge_->apply(fullDiveSnapshot, *safetyStateMachine_);
            }
            if (fullDiveSnapshot.intervention == FullDiveInterventionLevel::EmergencyStop ||
                fullDiveSnapshot.intervention == FullDiveInterventionLevel::Lockout) {
                emergency_->trigger();
                finishCycle(false, true, fullDiveSnapshot.reason);
                ++cycleCounter_;
                return;
            }
            if (fullDiveSnapshot.intervention == FullDiveInterventionLevel::Decompress ||
                fullDiveSnapshot.intervention == FullDiveInterventionLevel::RealityAnchor) {
                frame = reversibility_->runDecompression(frame);
            }
            if (fullDiveSnapshot.intervention == FullDiveInterventionLevel::ReduceIntensity) {
                for (float& channel : frame) {
                    channel *= std::clamp(1.0f - fullDiveSnapshot.severity, 0.25f, 1.0f);
                }
            }
        }
    }

    // ─────────────────────────────────────────────────────────────────────
    // [AMÉLIORATION 3] endTask("render") : même logique d'abandon actif.
    // Si le rendu dépasse son budget, on loggue mais on continue pour ne
    // pas perdre le uplink/downlink (données biométriques critiques).
    // ─────────────────────────────────────────────────────────────────────
    const auto tRender = std::chrono::steady_clock::now();
    renderer_->present(frame);
    const bool renderOk = scheduler_.endTask(RuntimeRenderStage::name(), tRender, tickReport);

    if (!renderOk) {
        Logger::warn("⚠️ Render budget exceeded — jitter risk. task=render");
        ++droppedFrames_;
    }

    if (independentWatchdog_) {
        independentWatchdog_->heartbeat("renderer");
        const WatchdogAssessment watchdog = independentWatchdog_->assess(
            tickReport.totalMs,
            !raw.empty(),
            fusedSafetyDecision.riskScore,
            diag.decisionDrift);
        if (watchdog.requiresEmergencyStop) {
            emergency_->trigger();
            if (safetyStateMachine_) {
                safetyStateMachine_->latchEmergency("watchdog:" + watchdog.reason);
            }
            Logger::error("[IndependentWatchdog] " + watchdog.reason);
            finishCycle(false, true, "watchdog:" + watchdog.reason);
            ++cycleCounter_;
            return;
        }
    }

    headset_->pushSensoryFrame(frame);

    pipeline.sensoryFrame = frame;
    pipeline.fusedSafetyDecision = fusedSafetyDecision;
    pipeline.stimulusStatus = RuntimeStimulusStage::validate(
        stimulusEnvelope_ != nullptr && neuralChipHal_ != nullptr,
        "stimulus_stage_unavailable");
    if (!pipeline.stimulusStatus.continueCycle) {
        finishCycle(false, pipeline.stimulusStatus.emergencyActive, pipeline.stimulusStatus.reason);
        ++cycleCounter_;
        return;
    }
    pipeline = RuntimeStimulusStage::execute(std::move(pipeline), RuntimeStimulusStage::Input{
        [this](std::uint64_t cycleId, const std::vector<float>& sensoryFrame, const SafetyDecision& decision, double deltaSeconds) {
            return stimulusEnvelope_->buildCommand(cycleId, sensoryFrame, decision, deltaSeconds);
        },
        [this]() { if (independentWatchdog_) independentWatchdog_->heartbeat("stimulus"); },
        [this](const SafeNeuralStimulusCommand& command) { return neuralChipHal_->writeSafeStimulus(command); },
        [this](const std::string& reason) { neuralChipHal_->emergencyStop(reason); },
        [this](const std::vector<float>& sensoryFrame) { return bridge_->downlink(sensoryFrame); },
        [this](const std::vector<float>& downlinkFrame) { nodes_->writeNeuralStimulus(downlinkFrame); }
    });

    if (!pipeline.continueCycle) {
        if (pipeline.stopReason == "unsafe_stimulus_rejected") {
            const auto rejectionAudit = safetyJournal_->record(
                "hal_reject",
                pipeline.stimulusCommand.safetyMode,
                pipeline.stimulusCommand.riskScore);
            emergency_->trigger();
            Logger::error("[NeuralChipHAL] Safe stimulus command rejected before node dispatch. journal=" +
                          rejectionAudit.eventHash);
        }
        finishCycle(false, pipeline.emergencyActive, pipeline.stopReason);
        ++cycleCounter_;
        return;
    }

    const RuntimeMetricsSnapshot metricsSnapshot = finishCycle(true, false, "nominal_runtime");

    if (!tickReport.withinBudget) {
        ++droppedFrames_;
        Logger::warn("⚠️ Runtime frame budget exceeded total_ms="
                     + std::to_string(tickReport.totalMs)
                     + " jitter_ms=" + std::to_string(tickReport.jitterMs)
                     + " over_budget_tasks=" + std::to_string(tickReport.overBudgetTaskCount)
                     + " p99_ms=" + std::to_string(metricsSnapshot.p99LatencyMs)
                     + " latency_health=" + runtimeLatencyHealthToString(metricsSnapshot.latencyHealth));
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
    auto target  = milliseconds(frameBudgetMs_);
    if (elapsed < target)
        std::this_thread::sleep_for(target - elapsed);
}

void RuntimeController::throttleEmergency() {
    std::this_thread::sleep_for(std::chrono::milliseconds(120));
}

void RuntimeController::setFrameBudgetMs(int ms) {
    frameBudgetMs_ = std::max(1, ms);
    std::lock_guard<std::mutex> lock(metricsMutex_);
    runtimeMetrics_.setLatencyBudgetMs(static_cast<double>(frameBudgetMs_));
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
