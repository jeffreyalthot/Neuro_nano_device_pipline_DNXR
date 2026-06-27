# Human Use Lock Verification

> Confidentiality: Level 3 — NDA controlled unless otherwise stated.
> Evidence boundary: preclinical R&D software evidence only; simulation/HIL/hardware-bench readiness does not establish clinical safety, effectiveness, certification, or authorization for human use.


## Required verified state
- Human-use is blocked by default.
- Clinical/human-use build flags require signed evidence gates and cannot be enabled by CMake flag alone.
- Hardware-bench paths remain no-human and require bench-specific session evidence.
- Documentation may describe an unlock workflow only as a QMS/regulatory evidence path, not as authorization.

## Unlock prerequisites before any human-subject activity
| Prerequisite | Required record | Status |
| --- | --- | --- |
| Regulatory authorization | IDE/ITA or applicable authorization | Open |
| Ethics approval | IRB/REB approval and informed consent package | Open |
| Medical supervision | Physician/investigator signoff and monitoring plan | Open |
| Device safety evidence | Independent hardware/HIL/preclinical verification | Open |
| QMS signed evidence | Signed gate bound to version, commit, target mode, protocol, and risk acceptance | Open |

Approved phrasing: Preclinical R&D software evidence package. Simulation/HIL readiness scaffold. Not authorized for human use. Not clinically validated. Not a certified medical device.
