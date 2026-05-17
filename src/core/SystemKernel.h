#pragma once

#include <memory>
#include <atomic>
#include <functional>
#include <chrono>
#include <string>
#include <mutex>

namespace dnxr {

class RuntimeController;

/// Noyau temps réel du pipeline immersif.
/// Coordonne l'exécution du moteur, synchronise les cycles et contrôle la stabilité.
class SystemKernel {
public:
    SystemKernel();
    ~SystemKernel();

    /// Initialise et démarre le runtime XR complet.
    void boot();

    /// Exécute une itération unique du cycle principal (traitement + synchronisation).
    void runCycle();

    /// Lance la boucle d'exécution continue.
    void run();

    /// Stoppe le runtime proprement et notifie les observateurs.
    void stop() noexcept;

    /// Permet d'enregistrer un callback d'état système (monitoring temps réel).
    void setStatusCallback(const std::function<void(const std::string&)>& cb);

    // --- Paramétrage du cycle ---
    void setCycleDuration(int ms);
    int getCycleDuration() const;

    void setMaxCyclesPerSecond(int cps);
    int getMaxCyclesPerSecond() const;

    /// Active ou désactive l'arrêt automatique (sécurité laboratory).
    void enableGracefulShutdown(bool enabled);

private:
    void throttle(const std::chrono::high_resolution_clock::time_point& start);
    bool shouldAutoStop() const;
    void sendStatus(const std::string& msg) const;

private:
    std::atomic<bool> runtimeRunning_;
    std::atomic<bool> gracefulShutdown_;
    int cycleDurationMs_;
    int maxCyclesPerSecond_;
    uint64_t cycleCounter_;

    std::unique_ptr<RuntimeController> controller_;
    std::function<void(const std::string&)> statusCallback_;
    mutable std::mutex mtx_;
};

} // namespace dnxr
