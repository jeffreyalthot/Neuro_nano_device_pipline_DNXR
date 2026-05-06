#pragma once

#include "NeuralMeshProtocol.h"
#include <string>

namespace dnxr {

class PacketRouter {
public:
    /// Routes a packet to its destination and tracks the last successful hop.
    bool route(const MeshPacket& packet);
    /// Returns the address of the last routed hop.
    std::string lastHop() const;

private:
    std::string last_hop_;
};

} // namespace dnxr
