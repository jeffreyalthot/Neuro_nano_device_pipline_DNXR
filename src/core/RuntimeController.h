#pragma once

#include <string>
#include <cstdint>
#include <memory>
#include <mutex>
#include <atomic>
#include <chrono>
#include <functional>
#include <vector>

#include "NeuroRealtimeScheduler.h"
#include "../utils/DeterministicSessionReplay.h"
#include "../utils/RuntimeMetrics.h"
#include "../utils/RuntimeTraceRecorder.h"
#include "../network/SimulationCryptoTransport.h"
#include "../safety/NeuralIdentityVault.h"
#include "../security/RuntimeSecret.h"
#include "../runtime/FullDiveSynchronizationEngine.h"
#include "../runtime/RuntimePipelineFrame.h"

namespace dnxr {

/// Contrôleur d’exécution principal du pipeline immersif.
/// Coordonne les modules du noyau en une chaîne cohérente temps réel.
class RuntimeController {
public:
    RuntimeController();
    ~RuntimeController();

    /// Effectue un cycle complet de traitement (1 frame).
    void runSingleCycle(double timestamp);

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

    // Initialisation centralisée des modules
    void initializeModules();
    void shutdownModules();

private:
    std::atomic<bool> runtimeRunning_;
    int frameBudgetMs_;
    std::atomic<uint64_t> droppedFrames_;
    NeuroRealtimeScheduler scheduler_;
    RuntimeMetrics runtimeMetrics_;
    RuntimeTraceRecorder traceRecorder_;
    mutable std::mutex traceMutex_;
    mutable std::mutex metricsMutex_;
    DeterministicSessionReplay replay_;
    SimulationCryptoTransport crypto_;
    std::unique_ptr<class SessionSafetyJournal> safetyJournal_;
    NeuralIdentityVault identityVault_;
    std::atomic<uint64_t> cycleCounter_;
    std::chrono::steady_clock::time_point lastCycleStart_{};
    std::string lastState_;

    RuntimeSecret neuralTransportSecret_;
    std::function<void(const std::string&)> eventCallback_;

    // Pré-instanciations pour réutilisation inter-frames
    std::unique_ptr<class NanoNodeInterface> nodes_;
    std::unique_ptr<class HostComputerBridge> bridge_;
    std::unique_ptr<class XRHeadsetInterface> headset_;
    std::unique_ptr<class SignalStreamProcessor> dsp_;
    std::unique_ptr<class FeatureVectorizer> vec_;
    std::unique_ptr<class StateInferenceEngine> brain_;
    std::unique_ptr<class NeuralCalibrationEngine> calib_;
    std::unique_ptr<class CalibrationDriftMonitor> calibrationDriftMonitor_;
    std::unique_ptr<class ImmersionCore> immersion_;
    std::unique_ptr<class MultisensoryClock> multisensoryClock_;
    std::unique_ptr<class XRRenderer> renderer_;
    std::unique_ptr<class PhysiologicalGuard> guard_;
    std::unique_ptr<class EmergencyShutdown> emergency_;
    std::unique_ptr<class CognitiveReversibilitySystem> reversibility_;
    std::unique_ptr<class RealityAnchorSystem> realityAnchor_;
    std::unique_ptr<class EnergyThermalOptimizer> thermalOptimizer_;
    std::unique_ptr<class SafetyGovernor> safetyGovernor_;
    std::unique_ptr<class SafetyStateMachine> safetyStateMachine_;
    std::unique_ptr<class IndependentWatchdog> independentWatchdog_;
    std::unique_ptr<class NeuralStimulusSafetyEnvelope> stimulusEnvelope_;
    std::unique_ptr<class SimulatedNeuralChipHAL> neuralChipHal_;
    std::unique_ptr<class DeepImmersionPlatform> deepImmersion_;
    std::unique_ptr<dnxr::FullDiveSynchronizationEngine> fullDiveSynchronization_;
    std::unique_ptr<class FullDiveSafetyBridge> fullDiveBridge_;

    mutable std::mutex mtx_;
};

} // namespace dnxr
