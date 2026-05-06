#pragma once
#include <string>
#include <vector>
namespace dnxr {
struct MeshPacket { std::string source; std::string destination; std::vector<float> payload; };
class NeuralMeshProtocol { public: MeshPacket encode(const std::string& src,const std::string& dst,const std::vector<float>& signal) const; std::vector<float> decode(const MeshPacket& packet) const;};
}
