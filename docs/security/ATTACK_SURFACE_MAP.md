# Attack Surface Map

> Confidentiality: Level 3 — NDA controlled unless otherwise stated.
> Evidence boundary: preclinical R&D software evidence, simulation/HIL readiness, and controlled hardware-bench preparation. Not authorized for human use, not clinically validated, and not a certified medical device.


## Primary trust boundaries
| Boundary | Inputs | Security concern | Required control | Evidence target |
| --- | --- | --- | --- | --- |
| Build configuration | CMake flags, presets | Unsafe feature enablement | Fatal gates for human-use/clinical flags without signed evidence | Configure transcript |
| Runtime configuration | JSON/session manifests | Malformed or malicious options | Schema validation and safe defaults | Unit/integration tests |
| Replay ingestion | Replay files and logs | Injection, parser crashes, inconsistent evidence | Deterministic replay checks and fuzzing | Replay dossier |
| Hardware bench | Device transport, session manifests | Human-subject bypass or active stimulation misuse | `NO_HUMAN` manifests and emergency-stop checks | HIL/hardware bench report |
| QMS evidence | Signed manifests, hashes | Forged or stale evidence | Signature/hash verification and revocation review | Evidence manifest |
| Data room | ZIP/USB/exported artifacts | Unauthorized disclosure or tampering | NDA levels, access logs, SHA-256 manifest | Access log and manifest |

## Abuse cases to test
- Enable clinical/human-use CMake flags without required signed evidence.
- Provide a session manifest with `human_subject_present=true` in hardware-bench mode.
- Mutate replay/config JSON to trigger parser or state-machine anomalies.
- Replace an evidence artifact after manifest generation and verify hash mismatch is detected.
- Attempt to use placeholder/example signatures as production authorization.
