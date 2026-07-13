#include "hardware/RealNeuralChipProtocol.h"
#include "hardware/RealNeuralChipTransport.h"

#include <cassert>
#include <cstring>

namespace {

dnxr::DeviceIdentityEvidence validIdentity() {
  return {"device-1", "certificate-123456789", "fw-1", "signature-123456789", "cal-1", false};
}

dnxr::ChipCapabilities requestedCaps() {
  dnxr::ChipCapabilities caps;
  caps.canVisual = true;
  caps.canAuditory = true;
  caps.canHaptic = true;
  caps.canVestibular = true;
  caps.canProprioceptive = true;
  caps.supportsHardwareCutoff = true;
  caps.supportsClosedLoopFeedback = true;
  caps.electrodeCount = 64;
  caps.wireCount = 4;
  caps.maxStimulusChannels = 64;
  caps.maxSafeRateHz = 60.0f;
  return caps;
}

dnxr::SafeNeuralStimulusCommandFixed command(std::uint64_t sequence) {
  dnxr::SafeNeuralStimulusCommandFixed c;
  c.sequenceId = sequence;
  c.monotonicTimestampMs = static_cast<std::int64_t>(sequence * 10U);
  c.boundedChannels.try_push_back(0.10f);
  c.boundedChannels.try_push_back(0.20f);
  c.modalityIntensities[0] = 0.10f;
  c.aggregateSensoryLoad = 0.10f;
  c.intensityScale = 0.10f;
  c.riskScore = 0.01f;
  c.safetyMode = "simulation_transport_test";
  c.calibrationProfileId = "cal-1";
  c.fiveSenseCoverage = true;
  c.exposureBudgetOk = true;
  c.allowStimulusDispatch = true;
  return c;
}

} // namespace

int main() {
  dnxr::SimulatedNeuralChipTransport transport;
  assert(transport.discover("sim://chip").ok);
  assert(transport.connect("sim://chip").ok);
  assert(!transport.attest({}).ok);
  assert(transport.attest(validIdentity()).ok);
  dnxr::ChipCapabilities negotiated;
  assert(transport.negotiate(requestedCaps(), negotiated).ok);
  assert(negotiated.maxStimulusChannels == 64U);
  assert(transport.synchronizeClock(100).ok);
  assert(transport.openAuthenticatedSession("session-001").ok);

  dnxr::RealNeuralChipProtocol protocol;
  const auto packet1 = protocol.buildPacket(command(1), "session-001", "genesis");
  assert(transport.prepareCommand(packet1).ok);
  assert(transport.submitPreparedCommand(packet1).ok);
  const auto ack1 = transport.receiveAck(packet1.packetId, 100);
  assert(ack1.accepted);
  assert(ack1.applied);
  assert(ack1.packetId == packet1.packetId);
  const auto duplicateAck = transport.receiveAck(packet1.packetId, 100);
  assert(!duplicateAck.accepted);
  assert(duplicateAck.duplicate);
  const auto replayPrepare = transport.prepareCommand(packet1);
  assert(!replayPrepare.ok);
  assert(replayPrepare.status == dnxr::NeuralTransportOperationStatus::Duplicate);

  const auto packet2 = protocol.buildPacket(command(2), "session-001", packet1.packetHash);
  assert(transport.prepareCommand(packet2).ok);
  assert(transport.submitPreparedCommand(packet2).ok);
  const auto timeoutAck = transport.receiveAck(packet2.packetId, 0);
  assert(!timeoutAck.accepted);
  assert(timeoutAck.timeout);
  const auto lateAck = transport.receiveAck(packet2.packetId + 1U, 100);
  assert(!lateAck.accepted);
  assert(lateAck.stale);
  const auto ack2 = transport.receiveAck(packet2.packetId, 100);
  assert(ack2.accepted);
  assert(transport.readFeedback().operation.ok);

  assert(transport.emergencyStop("test_cutoff").ok);
  const auto packet3 = protocol.buildPacket(command(3), "session-001", packet2.packetHash);
  assert(transport.prepareCommand(packet3).ok);
  assert(!transport.submitPreparedCommand(packet3).ok);
  assert(transport.close().ok);

  dnxr::RealNeuralChipTransport realTransport;
  assert(realTransport.discover("pci://dnxr").ok);
  assert(!realTransport.connect("pci://dnxr").ok);
  const auto realSend = realTransport.send(packet1);
  assert(!realSend.accepted);
  assert(realSend.rejectedReason == "prepare_command_unsupported" ||
         realSend.rejectedReason == "authenticated_session_required" ||
         realSend.rejectedReason == "real_transport_protocol_driver_unavailable");
  return 0;
}
