# Clinical Test Readiness Package

## Purpose
This package defines the software evidence that must be reviewed before the project can be evaluated for Canada ITA or United States IDE clinical-test planning. It is not an authorization, approval, or claim of readiness for human testing.

## Scope
- C++17 simulation runtime, safety pipeline, full-dive capability model, HIL scaffold, runtime determinism utilities, and cybersecurity scaffolding.
- Readiness artifacts covering requirements, traceability, risk controls, HIL evidence, cybersecurity, validation planning, and jurisdiction-specific clinical submission planning.
- Excludes implanted hardware validation, biocompatibility, animal studies, clinical protocol approval, REB/IRB approval, manufacturing validation, and signed regulatory submissions.

## Current status
| Area | Current evidence state | Clinical-readiness impact |
| --- | --- | --- |
| Integrated simulation | Active unit/integration tests pass in normal CTest lane | Supports IntegratedSimulation only |
| HIL | Controller, emulator, fault injection, latency instrumentation, and HIL assessor exist | HILReady remains partial until scenario evidence is exported and reviewed |
| Full-dive capability evidence | Capability matrix and readiness assessor are present | Evidence level remains simulated/HIL-scaffold, not preclinical complete |
| Risk management | Hazard file and traceability links exist | Residual risk requires independent review and non-simulated evidence |
| Cybersecurity | Audit trail, threat event, SBOM, key lifecycle, and secure update policy exist | Cybersecurity evidence is structural, not penetration-test complete |
| Regulatory package | Canada/US/REB/IRB checklists exist | Submission-ready content remains incomplete |

## Required evidence
- Concrete software requirements with source, hazard, risk-control, test, and evidence links.
- HIL scenario report for each high-severity hazard, including latency p95/p99/worst and fault recovery outcome.
- Software verification and validation summary from normal and clinical-readiness validation lanes.
- Signed safety case, risk management review, cybersecurity review, and physician/regulatory review.
- Canada ITA and US IDE pathway assumptions reviewed by qualified regulatory experts.

## Available evidence
- `docs/TRACEABILITY_MATRIX.csv` links concrete requirements to source files, tests, hazards, controls, and evidence summaries.
- `docs/HIL_EVIDENCE_SUMMARY.md` identifies available HIL scaffold tests and missing scenario-level evidence.
- `docs/FULL_DIVE_EVIDENCE_SUMMARY.md` identifies capability evidence levels and blockers.
- `docs/CYBERSECURITY_EVIDENCE_SUMMARY.md` summarizes audit/update/key/SBOM evidence and missing cyber validation.
- `docs/PRECLINICAL_SUMMARY_REPORT.md` states that current evidence is not yet preclinical complete.

## Missing evidence
- Signed independent risk review and residual-risk acceptance.
- Exported per-session HIL reports with scenario-to-hazard-to-control coverage.
- Long-run readiness lane evidence with disabled critical tests enabled.
- Formal clinical protocol, investigator brochure/package, informed consent materials, monitoring plan, adverse-event reporting plan, and site readiness evidence.
- Hardware phantom qualification, physiological replay datasets, and bench validation beyond simulation.

## Linked artifacts
- `docs/CANADA_ITA_READINESS_CHECKLIST.md`
- `docs/US_IDE_READINESS_CHECKLIST.md`
- `docs/REB_IRB_READINESS_CHECKLIST.md`
- `docs/PRECLINICAL_SUMMARY_REPORT.md`
- `docs/SOFTWARE_SAFETY_CASE_SUMMARY.md`
- `docs/HIL_EVIDENCE_SUMMARY.md`
- `docs/FULL_DIVE_EVIDENCE_SUMMARY.md`
- `docs/CYBERSECURITY_EVIDENCE_SUMMARY.md`
- `qms/generated/ClinicalGapReport.md`

## Linked tests
- `dnxr_clinical_readiness_assessor_test`
- `dnxr_preclinical_readiness_assessor_test`
- `dnxr_readiness_cannot_skip_hil_test`
- `dnxr_readiness_requires_traceability_test`
- `dnxr_hil_fault_injection_test`
- `dnxr_hil_latency_budget_test`
- `dnxr_hil_emergency_stop_test`
- `dnxr_hil_phantom_disconnect_test`
- `dnxr_hil_recovery_sequence_test`
- Long-run lane: `dnxr_runtime_controller_pipeline_10k_cycles_test`, `dnxr_runtime_controller_100k_cycles_test`, `dnxr_vestibular_exposure_budget_long_test`, `dnxr_full_dive_intervention_window_long_test`

## Reviewer / status
- Document owner: QMS software lead.
- Review state: Structured draft requiring independent clinical, regulatory, cybersecurity, and safety review before any submission.
- Current readiness conclusion: not ClinicalTestReady; this package supports gap closure and expert review only.

## Next actions
1. Replace simulated/HIL scaffold evidence with protocol-bound bench and HIL run records.
2. Complete independent risk review and resolve open residual-risk decisions.
3. Execute the clinical-readiness validation lane with long-run tests enabled.
4. Attach signed reviewer approvals, protocol identifiers, and dated evidence records.

## 0.3.9 readiness update
- Traceability is now concrete with 190 requirements linked to hazards, risk controls, source files, tests, evidence artifacts, verification methods, reviewer placeholders, and residual-risk statements.
- Placeholder detection exists through `tools/qms/check_placeholder_content.py` to reduce false readiness claims and generic requirement text.
- Evidence artifact path validation exists through `tools/qms/check_evidence_artifacts.py`.
- Disabled readiness tests are identified as blockers through `tools/qms/check_disabled_tests_impact.py` unless executed in the clinical-readiness validation lane or formally justified.
- `qms/generated/ClinicalGapReport.md` and `qms/generated/ExtendedVVReport.md` are generated as draft QMS artifacts.
- This update improves RegulatoryPackageDraftReady evidence structure but does not achieve PreclinicalEvidenceComplete, ClinicalSubmissionReady, or ClinicalTestReady.
