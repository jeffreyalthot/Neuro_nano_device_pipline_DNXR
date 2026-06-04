# Software Delta Summary 0.4.0

## Purpose
This delta summary records the 0.4.0 transition from HIL-oriented scaffolding toward a testable preclinical software evidence architecture.

## Scope
The delta covers C++17 runtime, HIL, readiness, country-profile, evidence-model, generated-QMS, and documentation changes included in the 0.4.0 baseline.

## Current status
0.4.0 added concrete HIL scenario execution models and country readiness profiles, while clinical-test readiness remains blocked by persistent evidence, named review, physical HIL execution, cybersecurity evidence, and readiness-lane records.

## Added software capabilities
- HIL scenario definition, result capture, runner evaluation, session reporting, and risk coverage matrix support.
- Latency p95/p99/worst statistics for HIL scenario evidence.
- EvidenceArtifact, EvidenceStatus, EvidenceReviewState, and EvidenceRepository readiness primitives.
- Canada and United States readiness profiles to separate ITA and IDE evidence expectations.
- ClinicalReadinessValidationReport generation support for consolidated QMS gap reporting.
- Additional HIL scenario, multi-fault recovery, latency, country-readiness, and evidence-model tests.

## Impact summary
- HIL scaffold moved to a protocol-bound HIL scenario system.
- The software can evaluate scenario pass/fail logic against outcome, latency, recovery, emergency-stop, and artifact-completeness criteria.
- HIL evidence is not yet complete until generated artifacts are retained under qms/hil and linked to traceability.
- ClinicalSubmissionReady and ClinicalTestReady remain blocked.

## Required evidence
- Session reports under qms/hil/session_reports.
- Risk coverage matrices under qms/hil/risk_coverage.
- Latency reports under qms/hil/latency_reports.
- Physical phantom qualification and calibration records.
- Named independent reviewer sign-off for high-risk hazards.

## Available evidence
- Normal build and active CTest lane pass in the analyzed 0.4.0 run.
- HIL scenario runner and reporting unit tests pass in the normal lane.
- Required readiness documents and traceability paths are present in the 0.4.0 baseline.

## Missing evidence
- Executed physical HIL session artifacts.
- Named independent reviewer signatures.
- Clinical-readiness validation lane transcript.
- Cybersecurity penetration/vulnerability evidence.
- Country-specific clinical authorization package review.

## Linked artifacts
- docs/HIL_EVIDENCE_SUMMARY.md
- docs/HIL_EXECUTION_PROTOCOL.md
- docs/HIL_FAULT_INJECTION_MATRIX.md
- qms/generated/ClinicalReadinessValidationReport.md

## Linked tests
- tests/dnxr_hil_scenario_runner_test.cpp
- tests/dnxr_hil_session_report_test.cpp
- tests/dnxr_hil_risk_coverage_matrix_test.cpp
- tests/dnxr_hil_multi_fault_recovery_test.cpp
- tests/dnxr_hil_latency_p99_test.cpp

## Reviewer / status
Reviewer role: QMS software lead with independent HIL reviewer pending. Status: draft delta record, not a clinical-test authorization record.

## Next actions
Generate 0.4.1 persistent HIL artifacts, execute the readiness lane, attach reviewer sign-off records, and update traceability rows to link the generated HIL evidence artifacts.
