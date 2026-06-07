#pragma once

#include <cstdint>
#include <string>

namespace dnxr {

enum class NeuralChipAckTransportState { NotReceived, TransportAccepted, TransportRejected };
enum class NeuralChipAckFirmwareState { NotValidated, FirmwareValidated, FirmwareRejected };
enum class NeuralChipAckHardwareState { NotApplied, HardwareApplied, HardwareRejected };
enum class NeuralChipAckFeedbackState { NotConfirmed, FeedbackConfirmed, FeedbackRejected };

struct NeuralChipAck {
    std::uint32_t protocolVersion = 1;
    std::string sessionId;
    std::string deviceIdHash;
    std::string firmwareHash;
    std::string packetHash;
    std::string previousPacketHash;
    std::string ackHash;
    std::string signatureAlgorithm;
    std::string signature;
    std::int64_t monotonicTimestampMs = 0;
    float deviceClockDriftMs = 0.0f;
    std::uint32_t appliedChannelCount = 0;
    std::uint32_t rejectedChannelCount = 0;
    std::string hardwareCutoffReason;
    std::string thermalState = "nominal";
    std::string batteryOrPowerState = "nominal";
    std::string electrodeIntegrityState = "unknown";
    std::uint64_t feedbackFrameId = 0;

    NeuralChipAckTransportState transportState = NeuralChipAckTransportState::NotReceived;
    NeuralChipAckFirmwareState firmwareState = NeuralChipAckFirmwareState::NotValidated;
    NeuralChipAckHardwareState hardwareState = NeuralChipAckHardwareState::NotApplied;
    NeuralChipAckFeedbackState feedbackState = NeuralChipAckFeedbackState::NotConfirmed;

    std::uint64_t packetId = 0;
    bool packetAccepted = false;
    bool packetRejected = false;
    bool firmwareValidated = false;
    bool firmwareRejected = false;
    bool applied = false;
    bool notApplied = true;
    bool feedbackConfirmed = false;
    bool hardwareCutoffActive = false;
    float latencyMeasuredMs = 0.0f;
    float deviceTemperature = 37.0f;
    float signalQuality = 0.0f;
    bool feedbackAvailable = false;
    bool signedAck = false;
    std::string rejectReason;

    bool fullyAppliedAndFeedbackConfirmed() const {
        return packetAccepted && firmwareValidated && applied && feedbackConfirmed && signedAck;
    }
};

} // namespace dnxr
