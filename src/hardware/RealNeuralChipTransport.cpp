#include "RealNeuralChipTransport.h"
#include "DeviceIdentityAttestation.h"

namespace dnxr {

NeuralTransportOperationResult ClinicalLockedNeuralChipTransport::emergencyStop(const std::string&) {
    return ok("clinical_locked_transport_cutoff_asserted");
}

NeuralTransportOperationResult SimulatedNeuralChipTransport::discover(const std::string& selector) {
    return selector.empty() ? rejected("simulation_selector_missing") : ok("simulation_transport_discovered");
}

NeuralTransportOperationResult SimulatedNeuralChipTransport::connect(const std::string& endpoint) {
    if (endpoint.empty()) return rejected("simulation_endpoint_missing");
    connected_ = true;
    cutoff_ = false;
    return ok("simulation_transport_connected");
}

NeuralTransportOperationResult SimulatedNeuralChipTransport::attest(const DeviceIdentityEvidence& evidence) {
    const auto decision = DeviceIdentityAttestation{}.verify(evidence);
    return decision.trusted ? ok("simulation_attestation_verified") : rejected("simulation_attestation_rejected");
}

NeuralTransportOperationResult SimulatedNeuralChipTransport::negotiate(const ChipCapabilities& requested, ChipCapabilities& negotiated) {
    if (!connected_) return {NeuralTransportOperationStatus::Unavailable, false, "transport_not_connected"};
    negotiated = ChipCapabilityNegotiation{}.clampForClinical(requested);
    if (negotiated.maxStimulusChannels == 0 || !negotiated.supportsHardwareCutoff || !negotiated.supportsClosedLoopFeedback) {
        return rejected("capability_negotiation_rejected");
    }
    return ok("capabilities_negotiated");
}

NeuralTransportOperationResult SimulatedNeuralChipTransport::synchronizeClock(std::int64_t hostMonotonicMs) {
    if (!connected_) return {NeuralTransportOperationStatus::Unavailable, false, "transport_not_connected"};
    return hostMonotonicMs > 0 ? ok("clock_synchronized") : rejected("invalid_host_clock");
}

NeuralTransportOperationResult SimulatedNeuralChipTransport::openAuthenticatedSession(const std::string& sessionId) {
    if (!connected_) return {NeuralTransportOperationStatus::Unavailable, false, "transport_not_connected"};
    if (sessionId.size() < 8U) return rejected("session_id_too_short");
    authenticated_ = true;
    return ok("authenticated_session_open");
}

NeuralTransportReadResult SimulatedNeuralChipTransport::read() {
    if (!connected_) return {{NeuralTransportOperationStatus::Unavailable, false, "transport_not_connected"}, {}};
    NeuralChipFrame frame;
    frame.sequenceId = submittedPacketId_ + 1U;
    frame.monotonicTimestampMs = static_cast<std::int64_t>(frame.sequenceId * 10U);
    frame.signalQuality = 1.0f;
    frame.channels = {0.1f, -0.1f};
    frame.simulation = true;
    return {ok("read_frame_available"), frame};
}

NeuralTransportOperationResult SimulatedNeuralChipTransport::prepareCommand(const RealNeuralChipPacket& packet) {
    if (!authenticated_) return {NeuralTransportOperationStatus::NotAuthenticated, false, "authenticated_session_required"};
    if (packet.packetId == 0 || packet.packetHash.empty() || packet.antiReplayNonce == 0) return rejected("packet_identity_or_integrity_missing");
    if (packet.packetId <= submittedPacketId_) return {NeuralTransportOperationStatus::Duplicate, false, "packet_replay_or_duplicate"};
    preparedPacketId_ = packet.packetId;
    return ok("command_prepared");
}

NeuralTransportOperationResult SimulatedNeuralChipTransport::submitPreparedCommand(const RealNeuralChipPacket& packet) {
    if (packet.packetId != preparedPacketId_) return rejected("packet_not_prepared");
    if (cutoff_) return rejected("hardware_cutoff_active");
    submittedPacketId_ = packet.packetId;
    return ok(packet.emergencyCutoff ? "emergency_packet_submitted" : "command_submitted_ack_pending");
}

NeuralTransportAck SimulatedNeuralChipTransport::receiveAck(std::uint64_t packetId, std::uint32_t timeoutMs) {
    if (timeoutMs == 0U) return {false, false, "ack_timeout", 0.0f, packetId, cutoff_, packetId, false, false, true};
    if (packetId == lastAckedPacketId_) return {false, false, "duplicate_ack", 0.0f, packetId, cutoff_, packetId, true, false, false};
    if (packetId != submittedPacketId_) return {false, false, "ack_packet_not_pending", 0.0f, packetId, cutoff_, packetId, false, true, false};
    lastAckedPacketId_ = packetId;
    return {true, true, "", 1.0f, packetId, cutoff_, packetId, false, false, false};
}

NeuralTransportFeedbackResult SimulatedNeuralChipTransport::readFeedback() {
    if (lastAckedPacketId_ == 0U) return {{NeuralTransportOperationStatus::Unavailable, false, "feedback_unavailable_before_ack"}, {}};
    NeuralFeedbackPacket feedback;
    feedback.feedbackFrameId = lastAckedPacketId_;
    feedback.packetHash = "simulated_feedback_hash";
    return {ok("feedback_available"), feedback};
}

NeuralTransportOperationResult SimulatedNeuralChipTransport::emergencyStop(const std::string&) {
    cutoff_ = true;
    return ok("hardware_cutoff_asserted");
}

NeuralTransportOperationResult SimulatedNeuralChipTransport::close() {
    connected_ = false;
    authenticated_ = false;
    return ok("transport_closed");
}

NeuralTransportOperationResult HILNeuralChipTransport::discover(const std::string& selector) { return delegate_.discover(selector); }
NeuralTransportOperationResult HILNeuralChipTransport::connect(const std::string& endpoint) { return delegate_.connect(endpoint); }
NeuralTransportOperationResult HILNeuralChipTransport::attest(const DeviceIdentityEvidence& evidence) { return delegate_.attest(evidence); }
NeuralTransportOperationResult HILNeuralChipTransport::negotiate(const ChipCapabilities& requested, ChipCapabilities& negotiated) { return delegate_.negotiate(requested, negotiated); }
NeuralTransportOperationResult HILNeuralChipTransport::synchronizeClock(std::int64_t hostMonotonicMs) { return delegate_.synchronizeClock(hostMonotonicMs); }
NeuralTransportOperationResult HILNeuralChipTransport::openAuthenticatedSession(const std::string& sessionId) { return delegate_.openAuthenticatedSession(sessionId); }
NeuralTransportOperationResult HILNeuralChipTransport::prepareCommand(const RealNeuralChipPacket& packet) { return delegate_.prepareCommand(packet); }
NeuralTransportOperationResult HILNeuralChipTransport::submitPreparedCommand(const RealNeuralChipPacket& packet) { return delegate_.submitPreparedCommand(packet); }
NeuralTransportAck HILNeuralChipTransport::receiveAck(std::uint64_t packetId, std::uint32_t timeoutMs) { return delegate_.receiveAck(packetId, timeoutMs); }
NeuralTransportOperationResult HILNeuralChipTransport::emergencyStop(const std::string& reason) { return delegate_.emergencyStop(reason); }

NeuralTransportOperationResult RealNeuralChipTransport::discover(const std::string& selector) {
    return selector.empty() ? rejected("real_transport_selector_missing") : ok("real_transport_descriptor_discovered");
}

NeuralTransportOperationResult RealNeuralChipTransport::connect(const std::string&) {
    return {NeuralTransportOperationStatus::Unavailable, false, "real_transport_protocol_driver_unavailable"};
}

NeuralTransportOperationResult RealNeuralChipTransport::emergencyStop(const std::string&) {
    return {NeuralTransportOperationStatus::Unavailable, false, "real_transport_cutoff_driver_unavailable"};
}

} // namespace dnxr
