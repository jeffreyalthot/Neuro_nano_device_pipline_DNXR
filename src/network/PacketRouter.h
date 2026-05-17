#pragma once

#include "NeuralMeshProtocol.h"
#include <string>
#include <cstdint>
#include <unordered_map>
#include <mutex>
#include <atomic>
#include <functional>
#include <chrono>

namespace dnxr {

/// Router de paquets du maillage neuron‑XR.
/// Fournit un routage logique entre noeuds du mesh distribué (simulation ou réel).
class PacketRouter {
public:
    PacketRouter();
    ~PacketRouter() = default;

    /// Route un paquet vers sa destination finale.
    /// Retourne true si routé avec succès.
    bool route(const MeshPacket& packet);

    /// Enregistre une règle de redirection.
    void registerRoute(const std::string& from, const std::string& to);

    /// Nettoie toutes les routes et statistiques.
    void resetStats();

    /// Accès aux statistiques runtime.
    uint64_t getRoutedCount() const;
    uint64_t getDroppedCount() const;
    std::string lastHop() const;

    /// Définit un callback pour tout événement de routage (diagnostic ou debug).
    void setOnRouteEvent(const std::function<void(const std::string&, bool)>& cb);

    /// Active/désactive le mode simulation.
    void enableSimulationMode(bool enabled);
    bool simulationModeEnabled() const noexcept;

private:
    bool isValidAddress(const std::string& addr) const;
    std::string resolveRoute(const std::string& dest);

private:
    mutable std::mutex routerMutex_;
    std::unordered_map<std::string, std::string> routingTable_;

    std::string lastHop_;
    std::atomic<uint64_t> packetsRouted_;
    std::atomic<uint64_t> droppedPackets_;
    std::atomic<bool> simulationMode_{true};

    std::function<void(const std::string&, bool)> onRouteEvent_;
};

} // namespace dnxr
