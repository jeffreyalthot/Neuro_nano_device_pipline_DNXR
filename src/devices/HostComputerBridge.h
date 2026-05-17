#pragma once

#include <string>
#include <vector>
#include <mutex>
#include <atomic>
#include <thread>
#include <functional>

namespace dnxr {

/// Configuration d’exécution de la passerelle hôte XR ↔ système neuromoteur.
struct HostBridgeRuntimeConfig {
    std::string upstreamEndpoint;   ///< endpoint pour envoi des données XR → host
    std::string downstreamEndpoint; ///< endpoint pour retour host → XR / stimulation
    std::string authToken;          ///< jeton de sécurité
    bool simulationMode{true};      ///< mode logique local (pas de requêtes réseau)
    float linkDelayMs{2.0f};        ///< latence moyenne simulée
    float jitterAmplitude{1.5f};    ///< bruit sur la latence
    float asyncRateHz{90.0f};       ///< fréquence de rafraîchissement si mode async
};

/// Pont hôte matériel/logique entre le moteur XR et une machine de calcul.
/// En mode réel, peut être connecté à un réseau ou SDK de communication.
class HostComputerBridge {
public:
    explicit HostComputerBridge(HostBridgeRuntimeConfig config);
    ~HostComputerBridge();

    HostComputerBridge(const HostComputerBridge&) = delete;
    HostComputerBridge& operator=(const HostComputerBridge&) = delete;
    HostComputerBridge(HostComputerBridge&&) noexcept = default;
    HostComputerBridge& operator=(HostComputerBridge&&) noexcept = default;

    /// Envoi des signaux vers le backend hôte (traitement des données)
    std::vector<float> uplink(const std::vector<float>& signal) const;

    /// Réception des commandes ou signaux corrigés depuis l’hôte
    std::vector<float> downlink(const std::vector<float>& command) const;

    /// Lancement d’un thread de communication continu (asynchrone).
    void startAsyncLink();
    void stopAsyncLink();
    bool isLinkActive() const noexcept;

    /// Callback pour réception temps réel (observateur externe).
    void setOnLinkEvent(const std::function<void(const std::string&)>& cb);

private:
    void validateRuntimeConfig() const;
    void runBackgroundLink();
    std::vector<float> synthesizeResponse(const std::vector<float>& input, float scale) const;

private:
    HostBridgeRuntimeConfig config_;
    std::atomic<bool> asyncRunning_{false};
    mutable std::mutex bridgeMutex_;
    std::thread asyncThread_;
    std::function<void(const std::string&)> linkEventCallback_;
};

} // namespace dnxr
