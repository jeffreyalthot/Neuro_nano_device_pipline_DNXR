#include "NeuralMeshProtocol.h"
namespace dnxr { MeshPacket NeuralMeshProtocol::encode(const std::string& src,const std::string& dst,const std::vector<float>& signal) const { return {src,dst,signal}; } std::vector<float> NeuralMeshProtocol::decode(const MeshPacket& packet) const { return packet.payload; } }
