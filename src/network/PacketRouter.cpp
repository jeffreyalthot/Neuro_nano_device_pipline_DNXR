#include "PacketRouter.h"
namespace dnxr { bool PacketRouter::route(const MeshPacket& packet){ last_hop_=packet.destination; return !packet.destination.empty(); } std::string PacketRouter::lastHop() const { return last_hop_; } }
