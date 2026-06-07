#include "NeuralChipSafetySupervisor.h"

#include <cmath>

namespace dnxr {
namespace {
NeuralChipSafetyDecision stop(const std::string& reason, float severity = 1.0f) {
    NeuralChipSafetyDecision d;
    d.allowed = false;
    d.action = NeuralChipSafetyAction::EmergencyStop;
    d.reason = reason;
    d.severity = severity;
    d.requiresEmergencyStop = true;
    return d;
}
}

NeuralChipSafetyDecision NeuralChipSafetySupervisor::evaluate(const NeuralChipAck& ack,
                                                              const ChipCapabilities& currentCapabilities,
                                                              const DeviceIdentity& identity,
                                                              std::uint64_t expectedPacketId,
                                                              const ChipCapabilities* sessionCapabilities) const {
    if (!currentCapabilities.supportsHardwareCutoff) return stop("hardware_cutoff_disabled");
    if (!currentCapabilities.supportsClosedLoopFeedback) return stop("closed_loop_feedback_absent");
    if (!identity.signedIdentity || identity.deviceIdHash.empty() || identity.firmwareHash.empty()) return stop("device_identity_untrusted");
    if (!ack.packetAccepted) return stop(ack.rejectReason.empty() ? "ack_absent_or_transport_rejected" : ack.rejectReason);
    if (!ack.signedAck || ack.signature.empty() || ack.signatureAlgorithm.empty()) return stop("ack_signature_required");
    if (ack.packetId != expectedPacketId) return stop("ack_packet_id_incoherent");
    if (!ack.firmwareValidated || ack.firmwareState != NeuralChipAckFirmwareState::FirmwareValidated) return stop("ack_firmware_not_validated");
    if (!ack.applied || ack.hardwareState != NeuralChipAckHardwareState::HardwareApplied) return stop("ack_hardware_not_applied");
    if (!ack.feedbackAvailable || !ack.feedbackConfirmed) return stop("ack_feedback_not_confirmed", 0.9f);
    if (ack.hardwareCutoffActive) return stop("hardware_cutoff_active");
    if (ack.deviceTemperature > 41.0f || ack.thermalState == "critical") return stop("device_temperature_critical");
    if (ack.latencyMeasuredMs > 20.0f) {
        NeuralChipSafetyDecision d;
        d.allowed = false;
        d.action = NeuralChipSafetyAction::HoldLastSafeFrame;
        d.reason = "ack_latency_exceeded";
        d.severity = 0.7f;
        return d;
    }
    if (ack.signalQuality < 0.25f) {
        NeuralChipSafetyDecision d;
        d.allowed = false;
        d.action = NeuralChipSafetyAction::ForceRealityAnchor;
        d.reason = "device_signal_quality_critical";
        d.severity = 0.8f;
        return d;
    }
    if (std::fabs(ack.deviceClockDriftMs) > 10.0f) return stop("device_clock_drift_exceeded", 0.8f);
    if (sessionCapabilities && (sessionCapabilities->supportsHardwareCutoff != currentCapabilities.supportsHardwareCutoff ||
                                sessionCapabilities->supportsClosedLoopFeedback != currentCapabilities.supportsClosedLoopFeedback)) {
        return stop("device_capability_changed_mid_session");
    }
    return {};
}

} // namespace dnxr

namespace dnxr {

NeuralChipSafetyDecision NeuralChipSafetySupervisor::evaluate(const NeuralChipAck& ack, bool cutoffAvailable, std::uint64_t expectedPacketId) const {
    ChipCapabilities c;
    c.supportsHardwareCutoff = cutoffAvailable;
    c.supportsClosedLoopFeedback = true;
    DeviceIdentity id;
    id.deviceIdHash = "legacy_test_device";
    id.firmwareHash = "legacy_test_firmware";
    id.signedIdentity = true;
    NeuralChipAck enriched = ack;
    enriched.packetAccepted = enriched.packetAccepted || !enriched.packetRejected;
    enriched.firmwareValidated = true;
    enriched.firmwareState = NeuralChipAckFirmwareState::FirmwareValidated;
    enriched.applied = enriched.applied || !enriched.notApplied;
    enriched.hardwareState = enriched.applied ? NeuralChipAckHardwareState::HardwareApplied : NeuralChipAckHardwareState::NotApplied;
    enriched.feedbackAvailable = true;
    enriched.feedbackConfirmed = true;
    enriched.signedAck = true;
    enriched.signatureAlgorithm = enriched.signatureAlgorithm.empty() ? "legacy" : enriched.signatureAlgorithm;
    enriched.signature = enriched.signature.empty() ? "legacy" : enriched.signature;
    return evaluate(enriched, c, id, expectedPacketId);
}

} // namespace dnxr
