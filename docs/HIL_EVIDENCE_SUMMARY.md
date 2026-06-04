# HIL Evidence Summary

## Purpose
Summarize hardware-in-the-loop scaffold evidence, known limitations, and required upgrades before preclinical-readiness review.

## Scope
Covers `src/hil` emulator/controller/fault/latency/evidence components and associated HIL CTest targets. Current evidence is simulated HIL scaffold evidence, not validated human-use or implanted-device evidence.

## Current status
| HIL capability | Current implementation | Evidence state | Gap |
| --- | --- | --- | --- |
| Session control | `HILController` runs sessions against phantom/emulator/load/fault inputs | Unit tested | Needs exported session protocol and result report |
| Neural signal emulation | `NeuralSignalEmulator` generates synthetic signals | Unit tested indirectly | Needs physiological replay datasets and dataset provenance |
| Stimulus load checking | `StimulusLoadSimulator` gates load budget | Unit tested | Needs calibrated stimulus bench instrumentation |
| Fault injection | `FaultInjectionEngine` injects emergency stop and phantom disconnect paths | Unit tested | Needs multi-fault recovery and coverage matrix |
| Latency instrumentation | `LatencyInstrumentation` reports latency budget outcomes | Unit tested | Needs p95/p99/worst per scenario and run provenance |
| Evidence recording | `HILEvidenceRecorder` records evidence summaries | Unit tested via assessor flow | Needs immutable per-run artifacts |

## Required evidence
- HIL scenario definitions with objective, hazard IDs, risk controls, setup, steps, expected outcome, and acceptance criteria.
- Per-session report containing phantom profile, software build identifier, latency p95/p99/worst, injected faults, recovery cycles, pass/fail decision, and operator/reviewer identity.
- Risk coverage matrix proving every high-severity hazard has at least one HIL or justified non-HIL verification path.
- Long-run recovery evidence for emergency stop, phantom disconnect, overload, and latency overrun scenarios.

## Available evidence
- `dnxr_hil_fault_injection_test` verifies the fault-injection path.
- `dnxr_hil_latency_budget_test` verifies latency budget handling.
- `dnxr_hil_emergency_stop_test` verifies emergency-stop HIL behavior.
- `dnxr_hil_phantom_disconnect_test` verifies phantom-disconnect behavior.
- `dnxr_hil_recovery_sequence_test` verifies recovery sequencing.
- `dnxr_hil_readiness_assessor_test` verifies readiness gating.

## Missing evidence
- HIL scenario runner and session report classes.
- HIL risk coverage matrix artifact.
- p95/p99 latency statistics by scenario.
- Physiological replay inputs and phantom qualification report.
- Independent reviewer sign-off.

## Linked artifacts
- `docs/HIL_TEST_PLAN.md`
- `docs/HIL_EXECUTION_PROTOCOL.md`
- `docs/HIL_FAULT_INJECTION_MATRIX.md`
- `docs/HIL_LATENCY_REPORT_TEMPLATE.md`
- `docs/HIL_SAFETY_ACCEPTANCE_CRITERIA.md`

## Linked tests
- `dnxr_hil_fault_injection_test`
- `dnxr_hil_latency_budget_test`
- `dnxr_hil_emergency_stop_test`
- `dnxr_hil_phantom_disconnect_test`
- `dnxr_hil_recovery_sequence_test`
- `dnxr_hil_readiness_assessor_test`

## Reviewer / status
- Document owner: QMS software lead.
- Review state: Structured draft requiring independent clinical, regulatory, cybersecurity, and safety review before any submission.
- Current readiness conclusion: not ClinicalTestReady; this package supports gap closure and expert review only.

## Next actions
1. Replace simulated/HIL scaffold evidence with protocol-bound bench and HIL run records.
2. Complete independent risk review and resolve open residual-risk decisions.
3. Execute the clinical-readiness validation lane with long-run tests enabled.
4. Attach signed reviewer approvals, protocol identifiers, and dated evidence records.

## Required future HIL artifacts
- `qms/hil/session_reports/*.md` for protocol-bound HIL scenario execution results.
- `qms/hil/risk_coverage_matrix.csv` for hazard/risk-control/scenario coverage.
- `qms/hil/latency_reports/*.md` for average, p95, p99, and worst-case latency by scenario.
- `qms/hil/phantom_qualification/*.md` for physiological replay inputs and phantom qualification.
- Independent reviewer sign-off records for each evidence-grade HIL session.
