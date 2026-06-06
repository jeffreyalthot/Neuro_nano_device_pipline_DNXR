#pragma once
#include <cstdint>
#include <string>
#include <vector>
namespace dnxr { struct RealNeuralChipPacket { std::uint64_t packetId=0; std::string sessionId; std::vector<float> payload; std::string commandHash; std::string previousCommandHash; bool emergencyCutoff=false; }; struct NeuralTransportAck { bool accepted=false, applied=false; std::string rejectedReason; float measuredLatencyMs=0.0f; std::uint64_t feedbackFrameId=0; bool hardwareCutoffState=false; }; class INeuralChipTransport { public: virtual ~INeuralChipTransport()=default; virtual NeuralTransportAck send(const RealNeuralChipPacket& packet)=0; virtual bool cutoff(const std::string& reason)=0; }; }
