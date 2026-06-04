# Readiness Delta 0.4.1

## Purpose
This document states how 0.4.1 changes the clinical-readiness posture and what still blocks ClinicalTestReady status.

## Scope
The scope is limited to software/QMS readiness for preclinical and regulatory expert review in Canada/US contexts. It is not a regulatory authorization or clinical-use claim.

## Current status
0.4.1 improves readiness by producing persistent HIL and QMS artifacts. ClinicalTestReady remains blocked.

## Readiness improvements
- HIL persistent session reports are available.
- HIL risk coverage CSV output is available.
- HIL latency reports are available.
- `HILScenarioEvidenceReport.md` is generated.
- QMS design/risk/release/verification/validation/security records exist for the 0.4.1 lane.
- QMS tooling can detect required document, traceability, placeholder-content, disabled-test, and evidence-artifact gaps.

## Remaining blockers
- Physical HIL evidence is missing.
- Named independent sign-off is missing.
- HIL coverage is partial and must expand to all hazards or include explicit non-HIL-applicability justifications.
- Cybersecurity evidence remains largely structural and requires executed scans, penetration testing, disposition tables, and signed review.
- The readiness lane needs a bounded 10k controller strategy so it completes reliably in CTest and produces persistent evidence.

## Required evidence
- `qms/generated/Runtime10kEvidence.md`
- Physical HIL bench report(s)
- Signed review records for high-risk hazards
- Cybersecurity scan/penetration evidence with disposition
- Canada/US pathway-review records accepted by qualified owners

## Available evidence
- `qms/hil/session_reports/HIL-SESSION-0.4.1.md`
- `qms/hil/risk_coverage/HIL-RISK-COVERAGE-0.4.1.csv`
- `qms/hil/latency_reports/HIL-LATENCY-0.4.1.md`
- `qms/generated/HILScenarioEvidenceReport.md`

## Missing evidence
- Target hardware latency and repeatability data.
- Named reviewer decision records.
- Complete HIL hazard coverage.
- Clinical/regulatory owner approvals.

## Linked artifacts
- `qms/validation_evidence/VAL-0.4.1-readiness-lane.md`
- `qms/problem_reports/PR-0.4.1-open-gaps.md`
- `qms/release_records/REL-0.4.1.md`

## Linked tests
- `dnxr_runtime_controller_pipeline_10k_cycles_test`
- `dnxr_vestibular_exposure_budget_long_test`
- `dnxr_full_dive_intervention_window_long_test`
- `dnxr_runtime_controller_100k_cycles_test`

## Reviewer / status
Reviewer role: clinical-readiness reviewer. Status: draft readiness delta pending named review.

## Next actions
1. Run the readiness lane after the bounded 10k controller update.
2. Update `ClinicalReadinessValidationReport.md` from generated HIL artifacts rather than stale scaffold assumptions.
3. Build ReviewGate/SignOffRecord support so unsigned artifacts cannot be treated as clinical-ready evidence.
