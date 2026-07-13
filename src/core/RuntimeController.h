#pragma once

#include <string>
#include <cstdint>
#include <memory>
#include <mutex>
#include <atomic>
#include <chrono>
#include <functional>
#include <vector>

#include "../config/RuntimeConfig.h"
#include "../config/RuntimeMode.h"
#include "../performance/FrameBudgetMonitor.h"

#include "NeuroRealtimeScheduler.h"
#include "../utils/DeterministicSessionReplay.h"
#include "../utils/RuntimeMetrics.h"
#include "../utils/RuntimeTraceRecorder.h"
#include "RuntimeCompositionRoot.h"
#include "../safety/NeuralIdentityVault.h"
#include "../security/RuntimeSecret.h"
#include "../runtime/FullDiveSynchronizationEngine.h"
#include "../runtime/RuntimePipelineFrame.h"
#include "../runtime/RuntimeCycleFinalizer.h"
#include "../runtime/RuntimeSafetyStage.h"
#include "../runtime/RuntimeFullDiveStage.h"
#include "../runtime/RuntimeStimulusStage.h"
#include "../runtime/RuntimeAcquisitionStage.h"
#include "../runtime/RuntimeSignalStage.h"
#include "../runtime/RuntimeInferenceStage.h"
#include "../runtime/RuntimeCalibrationStage.h"
#include "../runtime/RuntimeImmersionFrameStage.h"
#include "../runtime/CanonicalNeuralBciPipeline.h"
#include "../runtime/RuntimePathLock.h"
#include "../runtime/RuntimeStageServices.h"
#include "../runtime/RuntimePipelineSinks.h"

namespace dnxr {

/// Contrôleur d’exécution principal du pipeline immersif.
/// Coordonne les modules du noyau en une chaîne cohérente temps réel.
class RuntimeController {
public:
    RuntimeController();
    explicit RuntimeController(RuntimeConfig config);
    ~RuntimeController();

    /// Effectue un cycle complet de traitement (1 frame).
    void runSingleCycle(double timestamp);
    RuntimePipelineFrame runDeterministicCycle(double timestamp);

    /// Lance la boucle continue.
    void run();

    /// Stop le contrôleur proprement.
    void stop() noexcept;

    /// Définit la durée cible d’un frame (ms).
    void setFrameBudgetMs(int ms);
    int getFrameBudgetMs() const;

    /// Renvoie le dernier état cognitif connu.
    std::string getLastState() const;

    /// Expose quelques compteurs runtime.
    uint64_t getCycleCount() const;
    uint64_t getDroppedFrames() const;
    RuntimeMetricsSnapshot getRuntimeMetricsSnapshot() const;
    RuntimeLatencyHealth getRuntimeLatencyHealth() const;
    CanonicalPipelineMetrics getCanonicalPipelineMetrics() const;
    std::vector<RuntimeTraceEvent> getRuntimeTraceEvents() const;
    std::string getLatestRuntimeTraceId() const;
    bool saveRuntimeTraceJsonl(const std::string& path) const;

    /// Attache un callback pour rapporter les états du pipeline à un superviseur.
    void setRuntimeEventCallback(const std::function<void(const std::string&)>& cb);

private:
    void throttle(const std::chrono::high_resolution_clock::time_point& start);
    void throttleEmergency();
    bool shouldApplyRendererQoS(const RuntimeMetricsSnapshot& snapshot) const;
    void publishRuntimeMetrics(const RuntimeMetricsSnapshot& snapshot);
    void publishRuntimeEvent(const std::string& payload);
    double computeCycleDeltaSeconds(std::chrono::steady_clock::time_point cycleStart);
    RuntimeCycleFinalizer::Input makeCycleFinalizerInput();
    RuntimeCycleFinalizer::Context makeCycleFinalizerContext(const RuntimeMetricsSnapshot& previousMetricsSnapshot);
    void publishPipelineState(const RuntimePipelineFrame& pipeline);
    void publishRuntimeArtifacts(const RuntimePipelineFrame& pipeline);
    RuntimeAcquisitionStage::Input makeAcquisitionStageInput();
    RuntimeSignalStage::Input makeSignalStageInput();
    RuntimeInferenceStage::Input makeInferenceStageInput();
    RuntimeCalibrationStage::Input makeCalibrationStageInput();
    RuntimeSafetyStage::Input makeSafetyStageInput(double timestamp);
    RuntimeFullDiveStage::Input makeFullDiveStageInput(const RuntimeMetricsSnapshot& snapshot);
    RuntimeStimulusStage::Input makeStimulusStageInput();
    RuntimeStageServices makeRuntimeStageServices();
    RuntimePipelineSinks makeRuntimePipelineSinks();
    bool shouldStopAfterCycle() const;
    void emitShutdownDiagnostics() const;

    // Initialisation centralisée des modules
    void initializeModules();
    void shutdownModules();

private:
    RuntimeConfig config_;
    RuntimeMode runtimeMode_;
    mutable FrameBudgetMonitor frameBudgetMonitor_;
    std::atomic<bool> runtimeRunning_;
    int frameBudgetMs_;
    std::atomic<uint64_t> droppedFrames_;
    NeuroRealtimeScheduler scheduler_;
    RuntimeMetrics runtimeMetrics_;
    RuntimeTraceRecorder traceRecorder_;
    mutable std::mutex traceMutex_;
    mutable std::mutex metricsMutex_;
    DeterministicSessionReplay replay_;
    std::unique_ptr<class SessionSafetyJournal> safetyJournal_;
    std::string auditSessionPath_;
    std::unique_ptr<class RuntimeState> runtimeState_;
    std::unique_ptr<class CliDashboard> dashboard_;
    NeuralIdentityVault identityVault_;
    std::atomic<uint64_t> cycleCounter_;
    std::chrono::steady_clock::time_point lastCycleStart_{};
    std::string lastState_;

    RuntimeSecret neuralTransportSecret_;
    std::function<void(const std::string&)> eventCallback_;

    // Pré-instanciations pour réutilisation inter-frames
    std::unique_ptr<class NanoNodeInterface> nodes_;
    std::unique_ptr<class HostComputerBridge> bridge_;
    std::unique_ptr<class SignalStreamProcessor> dsp_;
    std::unique_ptr<class FeatureVectorizer> vec_;
    std::unique_ptr<class StateInferenceEngine> brain_;
    std::unique_ptr<class NeuralCalibrationEngine> calib_;
    std::unique_ptr<class CalibrationDriftMonitor> calibrationDriftMonitor_;
    std::unique_ptr<class MultisensoryClock> multisensoryClock_;
#if DNXR_SIMULATION_ONLY
    std::unique_ptr<class XRRenderer> observerRenderer_;
#endif
    std::unique_ptr<class ObserverMirrorSink> observerMirror_;
    std::unique_ptr<class PhysiologicalGuard> guard_;
    std::unique_ptr<class EmergencyShutdown> emergency_;
    std::unique_ptr<class CognitiveReversibilitySystem> reversibility_;
    std::unique_ptr<class RealityAnchorSystem> realityAnchor_;
    std::unique_ptr<class EnergyThermalOptimizer> thermalOptimizer_;
    std::unique_ptr<class SafetyGovernor> safetyGovernor_;
    std::unique_ptr<class SafetyStateMachine> safetyStateMachine_;
    std::unique_ptr<class IndependentWatchdog> independentWatchdog_;
    std::unique_ptr<class NeuralStimulusSafetyEnvelope> stimulusEnvelope_;
    std::unique_ptr<INeuralChipHAL> neuralChipHal_;
    std::unique_ptr<INeuralTransport> neuralTransport_;
    std::unique_ptr<INeuralPacketProtector> packetProtector_;
    std::unique_ptr<IBciAcquisitionSource> bciAcquisitionSource_;
    std::unique_ptr<IWorldIngress> worldIngress_;
    std::unique_ptr<IMotorWorldEgress> motorWorldEgress_;
    std::unique_ptr<IBiometricSource> biometricSource_;
    RuntimeDeploymentMode deploymentMode_ = RuntimeDeploymentMode::SimulationOnly;
    std::unique_ptr<class DeepImmersionPlatform> deepImmersion_;
    std::unique_ptr<dnxr::FullDiveSynchronizationEngine> fullDiveSynchronization_;
    std::unique_ptr<class FullDiveSafetyBridge> fullDiveBridge_;
    std::unique_ptr<CanonicalNeuralBciPipeline> canonicalPipeline_;
    BackendAdapterLock backendAdapterLock_;
    std::unique_ptr<RuntimeStageServices> stageServices_;
    std::unique_ptr<RuntimePipelineSinks> pipelineSinks_;

    mutable std::mutex mtx_;
};

} // namespace dnxr
