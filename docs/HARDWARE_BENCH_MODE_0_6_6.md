# Hardware Bench Mode 0.6.6

DNXR 0.6.6 supports a controlled `HARDWARE_BENCH_NO_HUMAN` path for real hardware transport testing without any human subject.

## Guardrails

- Human use remains forbidden unless `HumanClinicalTrialAuthorized` is selected with explicit authorization evidence.
- Dry-run is the default: the runtime may open the controlled HAL, handshake, read capabilities, verify ACK/telemetry paths, and measure latency, but it sends no active output.
- Active output before clinical authorization requires a `DUMMY_LOAD` or `PHANTOM` target and the dummy-load adapter.
- Emergency stop, safety kernel, trace recorder, hardware self-test, and QMS signature evidence are mandatory.
- `RealNeuralChipHAL` accepts only `BoundedStimulusCommand`; raw sensory/world renderer payloads do not compile against the real send API.
