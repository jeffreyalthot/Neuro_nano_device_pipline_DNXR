# Software Delta Summary 0.4.1

## Purpose
This summary identifies the software and generated-evidence delta introduced by 0.4.1.

## Scope
The scope includes HIL tools, generated evidence, QMS report generation, readiness-lane support, and the software limitations that remain before preclinical/clinical evaluation.

## Current status
0.4.1 establishes persistent HIL software evidence artifacts. It remains a software evidence lane rather than a physical bench or clinical submission package.

## Software changes
- HIL scenario execution tooling persists scenario outputs.
- HIL session-report generation now records scenario IDs, hazards, risk controls, pass/fail outcomes, latency metrics, operator role, reviewer role, and artifact paths.
- HIL risk coverage generation links hazards to scenarios and risk controls.
- HIL latency reporting records p99 latency against the configured budget.
- QMS generated reports are available for readiness, gap, extended V&V, traceability, and risk review.

## Impact
The software delta changes the evidence posture from “classes and planned reports exist” to “deterministic simulated HIL evidence can be generated and stored.” This improves auditability for software review, but it does not satisfy physical HIL, sign-off, or clinical authorization needs.

## Required evidence
- Full clinical-readiness lane execution with bounded 10k controller evidence.
- Physical HIL bench records and target hardware latency measurements.
- Named independent review and sign-off for high-risk hazards and clinical-readiness claims.

## Available evidence
- `qms/generated/HILScenarioEvidenceReport.md`
- `qms/generated/ClinicalReadinessValidationReport.md`
- `qms/generated/ExtendedVVReport.md`
- `qms/generated/ClinicalGapReport.md`

## Missing evidence
- Full 18-hazard HIL coverage or documented non-HIL-applicability.
- Signed cybersecurity and preclinical evidence dispositions.
- Executed long-run readiness lane transcript from the target environment.

## Linked artifacts
- `tools/qms/generate_clinical_readiness_validation_report.py`
- `tools/qms/run_clinical_readiness_lane.py`
- `tools/hil/run_hil_scenarios.py`

## Linked tests
- `dnxr_runtime_controller_pipeline_10k_cycles_test`
- `dnxr_runtime_controller_pipeline_smoke_test`
- `dnxr_runtime_controller_jitter_budget_test`

## Reviewer / status
Reviewer role: software V&V reviewer. Status: draft delta summary pending named review.

## Next actions
1. Keep the 10k controller lane bounded for routine CTest and allow explicit full mode with `DNXR_CONTROLLER_10K_FULL=1`.
2. Generate and attach `Runtime10kEvidence.md` whenever the readiness lane is run.
3. Promote generated artifacts through signed review gates before making readiness claims.
