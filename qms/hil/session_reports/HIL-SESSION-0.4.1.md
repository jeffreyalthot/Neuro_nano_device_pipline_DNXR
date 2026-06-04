# HIL Session Report 0.4.1

## Scope
This report persists protocol-bound HIL scenario outputs for the 0.4.1 evidence lane and links each result to hazards, risk controls, latency statistics, operator role, reviewer role, build ID, and source artifact path.

## Build metadata
- Software version: 0.4.1
- Build ID: DNXR-0.4.1-HIL-EVIDENCE-LANE
- Execution mode: deterministic simulated HIL evidence generation; physical bench execution remains required before clinical-test claims.
- Overall status: PASS

## Scenario results
| Scenario ID | Hazards | Risk controls | Phantom profile | Faults | Expected outcome | Observed outcome | Pass/fail | Average ms | p95 ms | p99 ms | Worst ms | Recovery cycles | Emergency stop | Operator | Reviewer | Timestamp | Artifact path |
| --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- | --- |
| HIL-SCEN-0.4.1-001 | H-001; H-003 | RC-001; RC-003 | closed-loop-neural-phantom-v0.4.1 | signal_dropout; latency_spike | safe degraded mode with stimulation inhibited | safe degraded mode with stimulation inhibited | PASS | 14.10 | 17.10 | 17.10 | 17.10 | 3/5 | PASS | HIL operator role - simulated execution | independent HIL reviewer role - pending named sign-off | 2026-06-04T00:00:00Z | qms/hil/session_reports/HIL-SESSION-0.4.1.md |
| HIL-SCEN-0.4.1-002 | H-004; H-008 | RC-004; RC-008 | vestibular-safety-phantom-v0.4.1 | phantom_disconnect; watchdog_stale_subsystem | emergency exit latched and operator acknowledgement required | emergency exit latched and operator acknowledgement required | PASS | 13.47 | 18.30 | 18.30 | 18.30 | 2/4 | PASS | HIL operator role - simulated execution | independent safety reviewer role - pending named sign-off | 2026-06-04T00:00:00Z | qms/hil/session_reports/HIL-SESSION-0.4.1.md |
| HIL-SCEN-0.4.1-003 | H-006; H-010 | RC-006; RC-010 | sensory-envelope-phantom-v0.4.1 | unsafe_profile_request; vestibular_budget_boundary | unsafe exposure rejected before stimulus delivery | unsafe exposure rejected before stimulus delivery | PASS | 11.99 | 15.90 | 15.90 | 15.90 | 1/3 | PASS | HIL operator role - simulated execution | independent risk reviewer role - pending named sign-off | 2026-06-04T00:00:00Z | qms/hil/session_reports/HIL-SESSION-0.4.1.md |

## Clinical-readiness limitations
- These generated records make the HIL evidence chain persistent, but do not replace named independent review, physical phantom calibration records, or country-specific clinical authorization review.
- Reviewer roles are recorded so the sign-off gate can block promotion until named reviewers accept residual risk.
