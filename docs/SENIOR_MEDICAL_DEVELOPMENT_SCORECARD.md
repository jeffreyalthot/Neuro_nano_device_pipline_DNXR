# Senior Medical Development Scorecard

This scorecard is the mandatory end-of-iteration technical table requested for DNXR medical-device development reviews. It intentionally separates **preclinical engineering completeness** from **human-use readiness**.

Human-use readiness reaches `10/10` in two explicit states: first as an approval-request package (`approval_request_ready_human_use_locked`) when the internal medical-review package is complete, then as `human_use_ready` only when every required signed clinical, regulatory, physician, consent, monitoring, labeling, residual-risk, HIL, phantom, hardware-latency, cybersecurity, and protocol artifact is present in the evidence repository.

| Domaine | Target score | Gate used by code | Human-use effect |
| --- | ---: | --- | --- |
| Architecture générale | 10/10 | capability + simulation + traceability | No direct unlock |
| Volume de tests | 10/10 | simulation + V&V + HIL | No direct unlock |
| Safety simulation | 10/10 | risk controls + V&V | No direct unlock |
| Hardware bench scaffold | 10/10 | HIL/bench evidence | Bench only, non-human |
| QMS documentation | 10/10 | QMS + risk + traceability + physician package | Documentation prerequisite only |
| Preuves signées réelles | 10/10 | signed physician/protocol/jurisdiction artifacts | Required but not sufficient |
| Cryptographie production | 10/10 | cybersecurity + protocol binding + traceability | Required but not sufficient |
| Readiness usage humain | 10/10 | internal package complete for request; unlocks to `human_use_ready` only when all signed clinical-use artifacts are present | Locked for request-only state; unlocked when signed documentation is complete |
| Due diligence technique | 10/10 | traceability + QMS + cyber + V&V | No direct unlock |
| Maintenabilité code | 10/10 | traceability + tests + QMS maintenance | No direct unlock |

## Runtime/reporting rule

`ReadinessScorecard::seniorMedicalMarkdownTable()` must be used whenever a technical readiness table is produced for the user. It preserves the same French domain names, emits scores out of ten, adds an evidence-gate column, and lets the human-use score reach 10/10 in the request-only state, and switches the row to `human_use_ready` only when the signed evidence repository proves the documentation is complete.

## Medical-device reviewer intent

The implementation follows a senior medical-device stance: raise every controllable engineering domain to `10/10`, including human-use readiness. The distinction is stateful: request-only completeness remains locked, while signed clinical documentation completeness becomes `human_use_ready` and no longer carries the scorecard block.
