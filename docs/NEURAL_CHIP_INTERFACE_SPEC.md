# Neural Chip Interface Specification

The real neural chip interface remains clinically locked. Prepared protocol components cover handshake evidence, capabilities, packet hashing, feedback format, emergency cutoff, latency reporting, firmware attestation, and calibration binding. Real transport refuses dispatch until ClinicalModeGate, DeviceIdentityAttestation, SignedEvidenceVerifier, and PhysicalEmergencyStop prerequisites are simultaneously satisfied.

## 0.5.1 Packet, ACK, and closed-loop requirements

Every real neural-chip packet carries:

- `protocolVersion`
- `sessionId`
- `sequenceId`
- `monotonicTimestamp`
- `patientCalibrationHash`
- `deviceCertificateHash`
- `firmwareHash`
- `commandType`
- `modalityMask`
- `channelCount`
- `payloadLength`
- `payloadHash`
- `previousPacketHash`
- `safetyMode`
- `emergencyCutoffState`
- `signature`

Every dispatch requires a `NeuralChipAck` containing packet accepted/rejected state, applied/not-applied state, hardware cutoff state, measured latency, device temperature, signal quality, feedback availability, signature status, and reject reason. Runtime stimulation fails closed when ACK is absent, too slow, unsigned, bound to the wrong packet id, reports active cutoff, or reports unsafe device temperature.

Closed-loop mode is mandatory outside pure simulation: command dispatch → ACK verification → neural feedback read → perception confidence → bounded correction → audit. Missing feedback cannot produce positive correction; it forces intensity reduction/reality anchoring and repeated timeout escalation.
