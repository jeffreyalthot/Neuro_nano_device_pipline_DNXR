# Preclinical Summary Report

## Purpose
State the current preclinical evidence position for software, HIL, safety, cybersecurity, and runtime determinism.

## Scope
This report covers repository evidence only. It excludes animal studies, cadaver/bench hardware validation outside the repository, implanted-device verification, sterilization/biocompatibility, and clinical data.

## Current status
| Readiness level | Status | Rationale |
| --- | --- | --- |
| SimulationOnly | Complete | Build and simulation tests are present |
| IntegratedSimulation | Supported | Runtime, full-dive, safety, security, and readiness tests exist |
| HILReady | Partial | HIL scaffold exists but scenario reports and risk coverage matrix are missing |
| PreclinicalEvidenceComplete | Not met | No signed non-simulated evidence package |
| ClinicalSubmissionReady | Not met | Protocol/regulatory package incomplete |
| ClinicalTestReady | Not met | No approval/authorization, clinical protocol, or completed preclinical evidence |

## Required evidence
- Executed HIL scenario suite with immutable session reports.
- Risk coverage matrix mapping hazards to controls and tests.
- Long-run validation lane output with all clinical-readiness tests enabled.
- Independent review of residual risks, cybersecurity, V&V, and physician-facing materials.

## Available evidence
- Normal build/test lane and 95 active CTest targets reported in the 0.3.8 review.
- Traceability matrix with 190 concrete requirements linked to source, tests, hazards, controls, and evidence summaries.
- HIL, full-dive, cybersecurity, and runtime summaries identify current evidence and gaps.

## Missing evidence
- Signed preclinical protocol and acceptance criteria.
- Bench/HIL exported result artifacts.
- Hardware phantom qualification.
- Physiological replay dataset provenance.
- Independent reviewer sign-off.

## Linked artifacts
- `docs/HIL_EVIDENCE_SUMMARY.md`
- `docs/FULL_DIVE_EVIDENCE_SUMMARY.md`
- `docs/CYBERSECURITY_EVIDENCE_SUMMARY.md`
- `docs/SOFTWARE_SAFETY_CASE_SUMMARY.md`
- `qms/generated/ExtendedVVReport.md`

## Linked tests
- Normal lane: `ctest --output-on-failure`
- Readiness lane: `ctest --test-dir build_readiness --output-on-failure` after enabling clinical-readiness options

## Reviewer / status
- Document owner: QMS software lead.
- Review state: Structured draft requiring independent clinical, regulatory, cybersecurity, and safety review before any submission.
- Current readiness conclusion: not ClinicalTestReady; this package supports gap closure and expert review only.

## Next actions
1. Replace simulated/HIL scaffold evidence with protocol-bound bench and HIL run records.
2. Complete independent risk review and resolve open residual-risk decisions.
3. Execute the clinical-readiness validation lane with long-run tests enabled.
4. Attach signed reviewer approvals, protocol identifiers, and dated evidence records.

## 0.3.9 HIL evidence limitation
HIL evidence remains scaffold-level until `HILScenarioRunner`, `HILSessionReport`, and protocol-bound HIL session artifacts are implemented, executed, reviewed, and linked to hazards, risk controls, requirements, and traceability rows.
