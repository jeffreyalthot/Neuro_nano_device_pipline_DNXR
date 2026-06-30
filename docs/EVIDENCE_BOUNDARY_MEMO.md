# Evidence Boundary Memo

> Confidentiality: Level 3 — NDA controlled unless otherwise stated.
> Evidence boundary: preclinical R&D software evidence only; simulation/HIL/hardware-bench readiness does not establish clinical safety, effectiveness, certification, or authorization for human use.


## Allowed evidence characterization
- Preclinical R&D software evidence package.
- Simulation and HIL readiness scaffold.
- Controlled hardware-bench preparation with no human subject.
- Architecture, traceability, risk, test, and QMS documentation prepared for expert review.

## Prohibited unsupported characterization
- Human-safety claim.
- Clinical-readiness or patient-validation claim.
- Implant-ready or approved/certified medical device.
- Full-dive human deployment ready.
- Authorized for clinical trial or human-subject use without external IDE/ITA/IRB/REB/physician approvals.

## Evidence classes
| Evidence class | Current meaning | Missing before clinical reliance |
| --- | --- | --- |
| Unit/integration tests | Software behavior checks in repository scope | Independent verification and traceable pass logs |
| HIL/replay evidence | Simulated/HIL scaffold and repeatability evidence | External HIL lab execution and protocol signoff |
| Hardware bench | No-human controlled bench preparation | Verified device-specific safety and no-human logs |
| QMS records | Internal readiness and review scaffolding | Quality-system owner approval and controlled records |
| Clinical mode gates | Software lock and authorization workflow | Signed regulatory/ethics/medical authorization package |
