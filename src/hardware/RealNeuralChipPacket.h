#pragma once

#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include "NeuralChipHAL.h"

namespace dnxr {

struct RealNeuralChipPacketHeader {
    std::uint32_t protocolVersion = 1;
    std::uint64_t packetId = 0;
    std::string sessionId;
    std::uint64_t sequenceId = 0;
    std::int64_t monotonicTimestamp = 0;
    std::uint64_t antiReplayNonce = 0;
    std::string previousPacketHash;
    std::string headerHash;
};

struct RealNeuralChipPayload {
    std::string commandType = "stimulus";
    FixedVector<float, DNXR_MAX_STIMULUS_CHANNELS> channels;
    std::array<float, DNXR_STIMULUS_MODALITY_COUNT> modalityIntensities{};
    std::uint32_t modalityMask = 0;
    std::uint32_t channelCount = 0;
    std::uint32_t payloadLength = 0;
    std::string payloadHash;
    std::string commandHash;
};

struct RealNeuralChipSafetyTrailer {
    std::string patientCalibrationHash;
    std::string deviceCertificateHash;
    std::string firmwareHash;
    std::string safetyMode;
    bool emergencyCutoff = false;
};

struct RealNeuralChipSignatureBlock {
    std::string signatureAlgorithm;
    std::string signature;
    std::string packetHash;
};

struct RealNeuralChipPacket {
    RealNeuralChipPacketHeader header;
    RealNeuralChipPayload body;
    RealNeuralChipSafetyTrailer safety;
    RealNeuralChipSignatureBlock signatureBlock;

    // Backward-compatible field aliases retained while INeuralChipTransport is being split.
    std::uint32_t protocolVersion = 1;
    std::uint64_t packetId = 0;
    std::string sessionId;
    std::uint64_t sequenceId = 0;
    std::int64_t monotonicTimestamp = 0;
    std::string patientCalibrationHash;
    std::string deviceCertificateHash;
    std::string firmwareHash;
    std::string commandType = "stimulus";
    std::string payloadHash;
    std::string headerHash;
    std::string packetHash;
    std::string previousPacketHash;
    std::string safetyMode;
    std::string signature;
    std::string signatureAlgorithm;
    std::uint64_t antiReplayNonce = 0;
    std::uint32_t modalityMask = 0;
    std::uint32_t channelCount = 0;
    std::uint32_t payloadLength = 0;
    FixedVector<float, DNXR_MAX_STIMULUS_CHANNELS> payload;
    std::array<float, DNXR_STIMULUS_MODALITY_COUNT> modalityIntensities{};
    std::string channelMapHash;
    std::string encodingModelSetHash;
    std::string targetMapHash;
    std::string calibrationDependencyHash;
    std::string safetyDecisionHash;
    std::string commandHash;
    std::string previousCommandHash;
    bool emergencyCutoff = false;
};

struct NeuralFeedbackPacket { std::uint64_t feedbackFrameId = 0; std::string packetHash; };

std::string computePacketHash(const RealNeuralChipPacket& packet);

} // namespace dnxr
