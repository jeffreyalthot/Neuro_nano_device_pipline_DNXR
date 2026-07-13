# DNXR Remaining External Dependencies — Prompt 18

Date: 2026-07-13 UTC.

The repository is structurally aligned around the canonical BCI runtime, but the following items cannot be proven inside this container without external systems.

| Dependency | Required To Prove | Current Runtime Behavior |
|---|---|---|
| Real neural chip transport protocol | Physical packet format, authenticated ACK, telemetry semantics, hardware error taxonomy | Real build compiles without simulation/HIL transport symbols and fails closed unless a clinical transport is supplied. |
| Hardware attestation service | Device identity, firmware provenance, signed capability evidence | Deployment evidence gates remain required; missing proof blocks real-human operation. |
| Signed QMS/readiness package | Not-for-human-use unlock, risk review, operator authorization | Human-use and clinical readiness locks remain closed by default. |
| Latency-certified deployment host | Bounded scheduling jitter, no-allocation lane, long-session timing | Local tests provide software evidence only; host certification remains external. |
| Patient-specific neural maps | Patient binding and modality map calibration | Simulation defaults are limited to non-human development adapters; production requires external patient binding. |
| Chemical channel authorization | Olfactory/gustatory experimental enablement | Chemical channels are disabled by default and remain blocked without explicit authorization. |

No external dependency is bypassed with an automatic ACK, simulated telemetry, null neural source or headset fallback in the real build.
