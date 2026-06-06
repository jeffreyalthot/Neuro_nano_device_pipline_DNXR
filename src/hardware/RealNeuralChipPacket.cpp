#include "RealNeuralChipPacket.h"
#include <functional>
namespace dnxr { std::string computePacketHash(const RealNeuralChipPacket& p){ std::size_t h=std::hash<std::string>{}(p.sessionId+p.previousCommandHash); h^=std::hash<std::uint64_t>{}(p.packetId); for(float v:p.payload) h^=std::hash<int>{}(static_cast<int>(v*100000.0f)); return std::to_string(h); } }
