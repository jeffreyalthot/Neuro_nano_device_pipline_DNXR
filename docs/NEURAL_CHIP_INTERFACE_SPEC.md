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

## 0.5.2 clinical chip negotiation, packet, ACK, and feedback requirements

Clinical sessions must negotiate chip capabilities through `INeuralChipHAL::capabilities()` before stimulus dispatch. Full neural-chip mode is refused when hardware cutoff or closed-loop feedback is absent. The session records a device identity/capability snapshot for audit binding.

Packets are represented as `RealNeuralChipPacketHeader`, `RealNeuralChipPayload`, `RealNeuralChipSafetyTrailer`, and `RealNeuralChipSignatureBlock`. Implementations must maintain separate header, payload, command, and packet hashes, carry the previous packet hash, include an anti-replay nonce, and sign with the clinical session signer. Placeholder signatures are only acceptable in simulation builds.

ACKs distinguish transport acceptance, firmware validation, hardware application, and feedback confirmation. ACK fields include protocol/session/device/firmware identity, packet hash chain fields, signature algorithm/signature, monotonic timestamp, device clock drift, applied/rejected channel counts, hardware cutoff reason, thermal and power state, electrode integrity, and feedback frame id. A transport-accepted ACK is not sufficient for clinical success.

Closed-loop feedback is per modality and must report visual, auditory, haptic, thermal, proprioceptive, vestibular, olfactory, gustatory, reality-anchor, and patient-stop confidence values before correction or continued stimulation.
