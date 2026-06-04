# Canada ITA Readiness Checklist

## Purpose
Track gaps that must be closed before asking qualified Canadian regulatory counsel and clinical investigators to evaluate an Investigational Testing Authorization pathway.

## Scope
This checklist covers software/HIL evidence and document readiness. It does not replace Health Canada classification, licensing, ethics, importation, labelling, or sponsor/manufacturer obligations.

## Current status
- Status: structure complete, evidence incomplete.
- Canada readiness profile: not ready for ITA submission review beyond preliminary gap assessment.
- Assumption requiring confirmation: investigational neuro-XR/full-dive software would require conservative device classification and human-subject protections because the simulated device could influence perception, cognition, sensory exposure, and emergency-exit behavior.

## Required evidence
| Category | Readiness criterion | Current state |
| --- | --- | --- |
| Device/software description | Architecture, intended use, indications, limitations, and simulation-only constraints documented | Partial; description exists but needs regulatory classification review |
| Investigational objective | Bench/HIL objectives separated from any clinical objective | Missing formal protocol |
| Manufacturer/importer roles | Accountable legal manufacturer/importer identified | Missing |
| Protocol readiness | Protocol, inclusion/exclusion, monitoring, stopping rules, and adverse-event reporting drafted | Missing |
| Risk management | Hazard analysis, risk controls, residual-risk review, and benefit-risk rationale signed | Draft; unsigned |
| Bench/HIL evidence | Scenario reports demonstrate hazard/control coverage and latency/recovery performance | Scaffold only |
| Cybersecurity | SBOM, threat model, secure update, key lifecycle, vulnerability plan, and test evidence complete | Structural only |
| Labelling/instructions | Investigator instructions, contraindications, warnings, emergency-exit instructions drafted | Missing |
| Investigator package | Physician-facing summary and evidence packet reviewed | Draft |
| Consent/REB | Consent content and REB submission package prepared | Missing |

## Available evidence
- Traceability matrix links requirements to hazards, controls, tests, and evidence summaries.
- HIL scaffold tests cover fault injection, emergency stop, phantom disconnect, recovery sequence, and latency budget.
- Readiness assessors prevent claiming clinical readiness without traceability/HIL evidence.

## Missing evidence
- Formal Canadian classification and ITA pathway memo.
- Signed clinical protocol, monitoring plan, consent materials, and investigator obligations.
- Dated HIL scenario reports, phantom qualification, and long-run validation records.
- Signed residual-risk evaluation and benefit-risk rationale.

## Linked artifacts
- `docs/CLINICAL_TEST_READINESS_PACKAGE.md`
- `docs/PRECLINICAL_SUMMARY_REPORT.md`
- `docs/REB_IRB_READINESS_CHECKLIST.md`
- `docs/HIL_EVIDENCE_SUMMARY.md`
- `docs/RESIDUAL_RISK_EVALUATION.md`

## Linked tests
- `dnxr_clinical_readiness_assessor_test`
- `dnxr_preclinical_readiness_assessor_test`
- `dnxr_hil_readiness_assessor_test`
- `dnxr_hil_recovery_sequence_test`

## Reviewer / status
- Document owner: QMS software lead.
- Review state: Structured draft requiring independent clinical, regulatory, cybersecurity, and safety review before any submission.
- Current readiness conclusion: not ClinicalTestReady; this package supports gap closure and expert review only.

## Next actions
1. Replace simulated/HIL scaffold evidence with protocol-bound bench and HIL run records.
2. Complete independent risk review and resolve open residual-risk decisions.
3. Execute the clinical-readiness validation lane with long-run tests enabled.
4. Attach signed reviewer approvals, protocol identifiers, and dated evidence records.
