# Secure Code Review Checklist

> Confidentiality: Level 3 — NDA controlled unless otherwise stated.
> Evidence boundary: preclinical R&D software evidence, simulation/HIL readiness, and controlled hardware-bench preparation. Not authorized for human use, not clinically validated, and not a certified medical device.


| ID | Review item | Pass criterion | Evidence | Status |
| --- | --- | --- | --- | --- |
| SCR-001 | Human-use/clinical flags | Cannot compile/run without signed QMS gate and external authorization record | CMake configure tests, clinical gate tests | Open |
| SCR-002 | Hardware bench no-human path | Requires no-human manifest and emergency-stop verification | HIL/hardware bench reports | Open |
| SCR-003 | Input parsing | JSON/config/replay parsers reject malformed or unsafe values | Unit tests, fuzzing plan | Open |
| SCR-004 | Cryptographic evidence | Hash/signature verification covers critical artifacts | Crypto review, evidence manifest | Open |
| SCR-005 | Secrets | No real keys/tokens/API credentials in repo or release archive | Secret scan transcript | Open |
| SCR-006 | Dependencies | SBOM, license matrix, SOUP supplier review complete | SPDX/CycloneDX/license files | Open |
| SCR-007 | Logging | Logs do not leak secrets or personal data | SAST/manual review | Open |
| SCR-008 | Claims | Documentation avoids clinical/safety overclaims | Claims review record | Open |
