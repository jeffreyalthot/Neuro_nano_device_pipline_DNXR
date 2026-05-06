#include "PacketRouter.h"

namespace dnxr {

bool PacketRouter::route(const MeshPacket& packet) {
    // This function validates destination metadata and tracks the last successfully routed hop.
    if (packet.destination.empty()) {
        return false;
    }

    last_hop_ = packet.destination;
    return true;
}

std::string PacketRouter::lastHop() const {
    // This function returns the most recent destination used by the routing layer.
    return last_hop_;
}

} // namespace dnxr
