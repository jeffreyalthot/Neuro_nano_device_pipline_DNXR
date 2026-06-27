# Due Diligence Master Scope

> Confidentiality: Level 3 — NDA controlled unless otherwise stated.
> Boundary: preclinical R&D due-diligence materials; not clinical validation or human-use authorization.


## Scope blocks
| Block | Purpose | Primary artifacts | Exit criterion |
| --- | --- | --- | --- |
| NDA/confidentiality | Control disclosure and reviewer access | NDA schedule, access log, classification matrix | Executed NDA and logged access for Level 2+ |
| Source integrity | Show what was disclosed and under which hash | Source manifest, SHA-256 master manifest, provenance register | Stable archive hash and source manifest reviewed |
| Build/test | Reproduce software evidence | Build report, CTest report, sanitizer reports | Complete logs attached and failures dispositioned |
| Cybersecurity/supply chain | Identify cyber and dependency risks | Attack surface, SBOM, SAST, fuzzing, vuln register | Tool-backed scans and license review complete |
| Validation | Map runtime/safety/replay/HIL/performance evidence | Validation evidence indexes | Independent reviewer can trace tests to evidence |
| Medical/QMS/regulatory | Bound claims and open clinical gaps | Claims/evidence maps, gap register, human-use lock record | No unsupported clinical/human-use claims |
| IP/legal | Establish ownership/provenance | Chain-of-title, contributor, third-party registers | Counsel-reviewed declarations signed |
| Commercial/acquisition | Support integration and transfer planning | Tech summary, feasibility matrix, transition/KT plans | Assumptions and exclusions recorded |

## Release boundary
The diligence package uses 0.6.5 as the medical-review baseline and 0.6.6 as an internal audit/hardware-bench increment. Human-use remains blocked unless the QMS/regulatory unlock evidence is completed.
