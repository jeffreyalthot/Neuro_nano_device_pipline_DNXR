#pragma once
#include "NeuralMeshProtocol.h"
#include <string>
namespace dnxr { class PacketRouter { public: bool route(const MeshPacket& packet); std::string lastHop() const; private: std::string last_hop_; }; }
