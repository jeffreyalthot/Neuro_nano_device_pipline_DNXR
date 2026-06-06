#pragma once
#include <cstdint>
#include <string>
#include <vector>
namespace dnxr {
struct RealNeuralChipPacket {
    std::uint32_t protocolVersion=1;
    std::uint64_t packetId=0;
    std::string sessionId;
    std::uint64_t sequenceId=0;
    std::int64_t monotonicTimestamp=0;
    std::string patientCalibrationHash,deviceCertificateHash,firmwareHash,commandType="stimulus",payloadHash,previousPacketHash,safetyMode,signature;
    std::uint32_t modalityMask=0,channelCount=0,payloadLength=0;
    std::vector<float> payload;
    std::string commandHash;
    std::string previousCommandHash;
    bool emergencyCutoff=false;
};
struct NeuralTransportAck { bool accepted=false, applied=false; std::string rejectedReason; float measuredLatencyMs=0.0f; std::uint64_t feedbackFrameId=0; bool hardwareCutoffState=false; };
class INeuralChipTransport { public: virtual ~INeuralChipTransport()=default; virtual NeuralTransportAck send(const RealNeuralChipPacket& packet)=0; virtual bool cutoff(const std::string& reason)=0; };
}
