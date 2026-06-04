# Change Impact Analysis 0.4.1

## Purpose
This record summarizes the 0.4.1 change impact relative to the prior scaffold-heavy evidence state and identifies the remaining blockers for Canada/US clinical-test evaluation.

## Scope
The analysis covers the HIL evidence lane, QMS generated artifacts, runtime-readiness lane behavior, traceability impact, security records, and residual clinical-readiness limitations.

## Current status
0.4.1 moves the repository from HIL software scaffolding toward persistent software evidence generation. It does not establish physical preclinical evidence, named independent sign-off, or clinical-test readiness.

## Changed items
- Added persistent HIL session reporting under `qms/hil/session_reports/`.
- Added persistent HIL risk coverage CSV output under `qms/hil/risk_coverage/`.
- Added persistent HIL latency reporting under `qms/hil/latency_reports/`.
- Added generated HIL scenario evidence under `qms/generated/HILScenarioEvidenceReport.md`.
- Added QMS design-review, risk-review, release-record, validation-evidence, verification-evidence, problem-report, and security records for the 0.4.1 evidence lane.
- Added HIL tooling for scenario execution and report generation under `tools/hil/`.

## Impact assessment
The primary impact is positive: HIL evidence is no longer only implied by classes and tests. The repository now stores reviewable artifacts that link scenarios to hazards, controls, outcomes, latency statistics, and reviewer-state limitations.

The clinical-readiness impact remains constrained because:
- HIL evidence is simulated software evidence, not physical bench evidence.
- Named independent sign-off remains pending.
- HIL risk coverage currently covers only a subset of hazards.
- The readiness lane required a bounded 10k controller strategy because the full controller-loop test can exceed routine CTest windows.

## Required evidence
- Physical HIL bench execution records.
- Phantom qualification and target-hardware timing records.
- Named operator/reviewer sign-off records.
- Full HIL hazard coverage or documented non-HIL-applicability justifications.
- Completed clinical-readiness lane transcript with runtime 10k evidence attached.

## Available evidence
- `qms/hil/session_reports/HIL-SESSION-0.4.1.md`
- `qms/hil/risk_coverage/HIL-RISK-COVERAGE-0.4.1.csv`
- `qms/hil/latency_reports/HIL-LATENCY-0.4.1.md`
- `qms/generated/HILScenarioEvidenceReport.md`
- `qms/validation_evidence/VAL-0.4.1-readiness-lane.md`

## Missing evidence
- Physical HIL execution logs and bench latency data.
- Signed independent review records.
- Complete 18-hazard HIL coverage or justification.
- Executed vulnerability/penetration evidence.
- Regulatory/clinical-owner acceptance for Canada/US pathway assumptions.

## Linked artifacts
- `tools/hil/run_hil_scenarios.py`
- `tools/hil/generate_hil_session_reports.py`
- `tools/hil/generate_hil_risk_coverage_matrix.py`
- `tools/hil/generate_hil_latency_report.py`
- `tools/qms/generate_clinical_readiness_validation_report.py`

## Linked tests
- `dnxr_runtime_controller_pipeline_smoke_test`
- `dnxr_runtime_controller_pipeline_10k_cycles_test`
- `dnxr_runtime_controller_100k_cycles_test`
- `dnxr_vestibular_exposure_budget_long_test`
- `dnxr_full_dive_intervention_window_long_test`

## Reviewer / status
Reviewer role: independent software safety reviewer. Status: draft impact analysis pending named review and sign-off.

## Next actions
1. Complete bounded readiness-lane execution and attach `qms/generated/Runtime10kEvidence.md`.
2. Expand HIL coverage to all hazards or document non-HIL-applicability.
3. Add signed review gates for high-risk evidence closure.
4. Replace placeholder cybersecurity evidence with executed scans and dispositions.
