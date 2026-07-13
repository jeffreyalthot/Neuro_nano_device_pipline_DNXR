#pragma once
#include "INeuralChipTransport.h"

namespace dnxr {

class ClinicalLockedNeuralChipTransport final : public INeuralChipTransport {
public:
    NeuralTransportOperationResult emergencyStop(const std::string& reason) override;
};

class SimulatedNeuralChipTransport final : public INeuralChipTransport {
public:
    NeuralTransportOperationResult discover(const std::string& selector) override;
    NeuralTransportOperationResult connect(const std::string& endpoint) override;
    NeuralTransportOperationResult attest(const DeviceIdentityEvidence& evidence) override;
    NeuralTransportOperationResult negotiate(const ChipCapabilities& requested, ChipCapabilities& negotiated) override;
    NeuralTransportOperationResult synchronizeClock(std::int64_t hostMonotonicMs) override;
    NeuralTransportOperationResult openAuthenticatedSession(const std::string& sessionId) override;
    NeuralTransportReadResult read() override;
    NeuralTransportOperationResult prepareCommand(const RealNeuralChipPacket& packet) override;
    NeuralTransportOperationResult submitPreparedCommand(const RealNeuralChipPacket& packet) override;
    NeuralTransportAck receiveAck(std::uint64_t packetId, std::uint32_t timeoutMs) override;
    NeuralTransportFeedbackResult readFeedback() override;
    NeuralTransportOperationResult emergencyStop(const std::string& reason) override;
    NeuralTransportOperationResult close() override;
private:
    bool connected_ = false;
    bool authenticated_ = false;
    bool cutoff_ = false;
    std::uint64_t preparedPacketId_ = 0;
    std::uint64_t submittedPacketId_ = 0;
    std::uint64_t lastAckedPacketId_ = 0;
};

class HILNeuralChipTransport final : public INeuralChipTransport {
public:
    NeuralTransportOperationResult discover(const std::string& selector) override;
    NeuralTransportOperationResult connect(const std::string& endpoint) override;
    NeuralTransportOperationResult attest(const DeviceIdentityEvidence& evidence) override;
    NeuralTransportOperationResult negotiate(const ChipCapabilities& requested, ChipCapabilities& negotiated) override;
    NeuralTransportOperationResult synchronizeClock(std::int64_t hostMonotonicMs) override;
    NeuralTransportOperationResult openAuthenticatedSession(const std::string& sessionId) override;
    NeuralTransportOperationResult prepareCommand(const RealNeuralChipPacket& packet) override;
    NeuralTransportOperationResult submitPreparedCommand(const RealNeuralChipPacket& packet) override;
    NeuralTransportAck receiveAck(std::uint64_t packetId, std::uint32_t timeoutMs) override;
    NeuralTransportOperationResult emergencyStop(const std::string& reason) override;
private:
    SimulatedNeuralChipTransport delegate_;
};

class RealNeuralChipTransport final : public INeuralChipTransport {
public:
    NeuralTransportOperationResult discover(const std::string& selector) override;
    NeuralTransportOperationResult connect(const std::string& endpoint) override;
    NeuralTransportOperationResult emergencyStop(const std::string& reason) override;
};

} // namespace dnxr
