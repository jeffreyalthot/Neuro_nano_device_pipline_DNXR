#pragma once

#include "RealNeuralChipPacket.h"
#include "ChipCapabilityNegotiation.h"
#include "NeuralChipHAL.h"
#include "DeviceIdentityAttestation.h"

#include <cstdint>
#include <string>

namespace dnxr {

enum class NeuralTransportOperationStatus {
    Ok,
    Unsupported,
    Unavailable,
    Rejected,
    Timeout,
    Duplicate,
    Stale,
    ProtocolError,
    NotAuthenticated
};

struct NeuralTransportOperationResult {
    NeuralTransportOperationStatus status = NeuralTransportOperationStatus::Unsupported;
    bool ok = false;
    std::string reason = "unsupported";
};

struct NeuralTransportAck {
    bool accepted = false;
    bool applied = false;
    std::string rejectedReason;
    float measuredLatencyMs = 0.0f;
    std::uint64_t feedbackFrameId = 0;
    bool hardwareCutoffState = false;
    std::uint64_t packetId = 0;
    bool duplicate = false;
    bool stale = false;
    bool timeout = false;
};

struct NeuralTransportReadResult {
    NeuralTransportOperationResult operation;
    NeuralChipFrame frame;
};

struct NeuralTransportFeedbackResult {
    NeuralTransportOperationResult operation;
    NeuralFeedbackPacket feedback;
};

class INeuralChipTransport {
public:
    virtual ~INeuralChipTransport() = default;

    virtual NeuralTransportOperationResult discover(const std::string& selector) { (void)selector; return unsupported("discover_unsupported"); }
    virtual NeuralTransportOperationResult connect(const std::string& endpoint) { (void)endpoint; return unsupported("connect_unsupported"); }
    virtual NeuralTransportOperationResult attest(const DeviceIdentityEvidence& evidence) { (void)evidence; return unsupported("attest_unsupported"); }
    virtual NeuralTransportOperationResult negotiate(const ChipCapabilities& requested, ChipCapabilities& negotiated) { (void)requested; negotiated = {}; return unsupported("negotiate_unsupported"); }
    virtual NeuralTransportOperationResult synchronizeClock(std::int64_t hostMonotonicMs) { (void)hostMonotonicMs; return unsupported("clock_sync_unsupported"); }
    virtual NeuralTransportOperationResult openAuthenticatedSession(const std::string& sessionId) { (void)sessionId; return unsupported("authenticated_session_unsupported"); }
    virtual NeuralTransportReadResult read() { return {unsupported("read_unsupported"), {}}; }
    virtual NeuralTransportOperationResult prepareCommand(const RealNeuralChipPacket& packet) { (void)packet; return unsupported("prepare_command_unsupported"); }
    virtual NeuralTransportOperationResult submitPreparedCommand(const RealNeuralChipPacket& packet) { (void)packet; return unsupported("submit_command_unsupported"); }
    virtual NeuralTransportAck receiveAck(std::uint64_t packetId, std::uint32_t timeoutMs) { (void)packetId; (void)timeoutMs; return {false, false, "ack_unsupported", 0.0f, 0, false, packetId, false, false, false}; }
    virtual NeuralTransportFeedbackResult readFeedback() { return {unsupported("feedback_unsupported"), {}}; }
    virtual NeuralTransportOperationResult emergencyStop(const std::string& reason) { (void)reason; return unsupported("emergency_stop_unsupported"); }
    virtual NeuralTransportOperationResult close() { return unsupported("close_unsupported"); }

    virtual NeuralTransportAck send(const RealNeuralChipPacket& packet) {
        const auto prepared = prepareCommand(packet);
        if (!prepared.ok) return {false, false, prepared.reason, 0.0f, 0, packet.emergencyCutoff, packet.packetId, false, false, false};
        const auto submitted = submitPreparedCommand(packet);
        if (!submitted.ok) return {false, false, submitted.reason, 0.0f, 0, packet.emergencyCutoff, packet.packetId, false, false, false};
        return receiveAck(packet.packetId, 100);
    }
    virtual bool cutoff(const std::string& reason) { return emergencyStop(reason).ok; }

protected:
    static NeuralTransportOperationResult unsupported(const std::string& reason) {
        return {NeuralTransportOperationStatus::Unsupported, false, reason};
    }
    static NeuralTransportOperationResult ok(const std::string& reason) {
        return {NeuralTransportOperationStatus::Ok, true, reason};
    }
    static NeuralTransportOperationResult rejected(const std::string& reason) {
        return {NeuralTransportOperationStatus::Rejected, false, reason};
    }
};

} // namespace dnxr
