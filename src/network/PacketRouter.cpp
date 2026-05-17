#include "PacketRouter.h"
#include "../utils/Logger.h"

#include <algorithm>
#include <regex>

namespace dnxr {

PacketRouter::PacketRouter()
    : lastHop_("unknown"), packetsRouted_(0), droppedPackets_(0) {}

bool PacketRouter::isValidAddress(const std::string& addr) const {
    // autorise a-z, 0-9, ._:/-
    static const std::regex pattern(R"(^[A-Za-z0-9._:/-]+$)");
    return !addr.empty() && std::regex_match(addr, pattern);
}

std::string PacketRouter::resolveRoute(const std::string& dest) {
    auto it = routingTable_.find(dest);
    if (it != routingTable_.end())
        return it->second;
    return dest;
}

bool PacketRouter::route(const MeshPacket& packet) {
    std::lock_guard<std::mutex> lock(routerMutex_);

    if (packet.destination.empty() || !isValidAddress(packet.destination)) {
        ++droppedPackets_;
        Logger::warn("[PacketRouter] Invalid destination: " + packet.destination);
        if (onRouteEvent_) onRouteEvent_(packet.destination, false);
        return false;
    }

    const std::string resolved = resolveRoute(packet.destination);
    lastHop_ = resolved;
    ++packetsRouted_;

    if (simulationMode_) {
        Logger::info("[PacketRouter] (SIM) Routed packet -> " + resolved);
    } else {
        // Point d’intégration matériel / réseau réel
        // Exemple :
        // NetworkBridge::send(packet.payload, resolved);
    }

    if (onRouteEvent_) onRouteEvent_(resolved, true);
    return true;
}

void PacketRouter::registerRoute(const std::string& from, const std::string& to) {
    std::lock_guard<std::mutex> lock(routerMutex_);
    routingTable_[from] = to;
    Logger::info("[PacketRouter] Registered route " + from + " -> " + to);
}

void PacketRouter::resetStats() {
    packetsRouted_ = 0;
    droppedPackets_ = 0;
    std::lock_guard<std::mutex> lock(routerMutex_);
    routingTable_.clear();
}

uint64_t PacketRouter::getRoutedCount() const { return packetsRouted_.load(); }
uint64_t PacketRouter::getDroppedCount() const { return droppedPackets_.load(); }

std::string PacketRouter::lastHop() const {
    std::lock_guard<std::mutex> lock(routerMutex_);
    return lastHop_;
}

void PacketRouter::setOnRouteEvent(const std::function<void(const std::string&, bool)>& cb) {
    onRouteEvent_ = cb;
}

void PacketRouter::enableSimulationMode(bool enabled) {
    simulationMode_ = enabled;
    Logger::info(std::string("[PacketRouter] Simulation mode ") +
                 (enabled ? "enabled" : "disabled"));
}

bool PacketRouter::simulationModeEnabled() const noexcept { return simulationMode_; }

} // namespace dnxr
