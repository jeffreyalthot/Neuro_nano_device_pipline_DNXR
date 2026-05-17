#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <mutex>
#include <atomic>
#include <functional>
#include <thread>

namespace dnxr {

/// Configuration d’exécution pour le cluster NanoNode.
struct NanoNodeRuntimeConfig {
    std::string endpoint;                ///< adresse réseau du backend
    std::string authToken;               ///< jeton d’authentification sécurisé
    std::size_t expectedNodeCount{100000};
    bool simulationMode{true};           ///< true = données synthétiques, false = interface réelle
    float updateRateHz{120.0f};          ///< cadence d’acquisition
};

/// Interface d’acquisition I/O neuronale.
/// Fournit un pont sûr entre le moteur XR et un cluster neuronal simulé ou matériel.
class NanoNodeInterface {
public:
    explicit NanoNodeInterface(NanoNodeRuntimeConfig config);
    ~NanoNodeInterface();

    NanoNodeInterface(const NanoNodeInterface&) = delete;
    NanoNodeInterface& operator=(const NanoNodeInterface&) = delete;
    NanoNodeInterface(NanoNodeInterface&&) noexcept = default;
    NanoNodeInterface& operator=(NanoNodeInterface&&) noexcept = default;

    /// Lecture du flux brut. En simulation, génère un signal pseudo‑neural.
    std::vector<float> readNeuralNodeSignals() const;

    /// Écrit une stimulation calibrée vers le backend.
    void writeNeuralStimulus(const std::vector<float>& vec) const;

    /// Démarre un thread asynchrone de flux continu (pour usage temps réel).
    void startAsyncStream();
    void stopAsyncStream();

    bool isStreamActive() const noexcept;
    bool simulationModeEnabled() const noexcept;

    void setOnSignalUpdate(const std::function<void(const std::vector<float>&)>& cb);

private:
    void validateRuntimeConfig() const;
    void simulateNodeStream(std::vector<float>& out, std::size_t count) const;
    void runBackgroundLoop();

private:
    NanoNodeRuntimeConfig config_;
    mutable std::mutex bufferMutex_;
    mutable std::vector<float> lastNodeBuffer_;

    std::atomic<bool> streamRunning_{false};
    std::thread streamThread_;
    std::function<void(const std::vector<float>&)> onSignalUpdate_;
};

} // namespace dnxr
