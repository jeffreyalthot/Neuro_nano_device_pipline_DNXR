# United States IDE Readiness Checklist

## Purpose
Track software, HIL, cybersecurity, risk, and quality-system gaps before expert evaluation of a United States Investigational Device Exemption pathway.

## Scope
This checklist covers preclinical software-readiness artifacts. It does not assert IDE eligibility, significant-risk/non-significant-risk status, IRB approval, sponsor responsibilities, or FDA acceptance.

## Current status
- Status: draft structure complete; evidence is not IDE-submission ready.
- US readiness profile: preliminary gap-assessment only.
- Critical caveat: clinical use cannot be inferred from passing simulation tests; safety/effectiveness evidence for an investigational device requires protocol-bound evidence and regulatory review.

## Required evidence
| Category | Readiness criterion | Current state |
| --- | --- | --- |
| Device description | Software architecture, intended use, operating principles, warnings, and limits | Partial |
| Prior investigations | Bench/HIL/preclinical evidence summarized with objective acceptance criteria | Scaffold only |
| Risk analysis | Hazard analysis and risk controls linked to verification and residual-risk decisions | Draft; unsigned |
| Clinical protocol | Study design, endpoints, stopping rules, monitoring, informed consent, and investigator responsibilities | Missing |
| Cybersecurity | SBOM, threat model, secure update/key lifecycle, vulnerability management, and test evidence | Structural only |
| Software lifecycle | Requirements, architecture, detailed design, V&V, release, maintenance, problem resolution, and configuration management | Draft structure present |
| Manufacturing/configuration | Build provenance, configuration baseline, SOUP inventory, release controls | Partial |
| Human-subject protections | IRB packet, consent, privacy/data handling, adverse-event reporting | Missing |

## Available evidence
- Normal build and active CTest lane can be used as baseline software verification evidence.
- Runtime latency/jitter/WCET/no-allocation components provide initial determinism instrumentation.
- Security tests cover replay attack, packet tamper detection, audit trail, threat event, and secure update policy.

## Missing evidence
- IDE pathway memo and significant-risk assessment by qualified counsel/regulatory experts.
- Executed extended validation lane with long tests enabled.
- Signed clinical protocol, investigator agreement package, monitoring plan, and IRB-ready consent.
- Penetration-test report, vulnerability disclosure process execution evidence, and unresolved vulnerability disposition.

## Linked artifacts
- `docs/CLINICAL_TEST_READINESS_PACKAGE.md`
- `docs/SOFTWARE_REQUIREMENTS_SPECIFICATION.md`
- `docs/SOFTWARE_VERIFICATION_PLAN.md`
- `docs/SOFTWARE_VALIDATION_PLAN.md`
- `docs/CYBERSECURITY_EVIDENCE_SUMMARY.md`
- `docs/SBOM.md`

## Linked tests
- `dnxr_secure_update_policy_test`
- `dnxr_replay_attack_blocked_test`
- `dnxr_packet_tamper_detection_test`
- `dnxr_runtime_latency_profiler_test`
- `dnxr_runtime_jitter_analyzer_test`
- `dnxr_runtime_wcet_recorder_test`

## Reviewer / status
- Document owner: QMS software lead.
- Review state: Structured draft requiring independent clinical, regulatory, cybersecurity, and safety review before any submission.
- Current readiness conclusion: not ClinicalTestReady; this package supports gap closure and expert review only.

## Next actions
1. Replace simulated/HIL scaffold evidence with protocol-bound bench and HIL run records.
2. Complete independent risk review and resolve open residual-risk decisions.
3. Execute the clinical-readiness validation lane with long-run tests enabled.
4. Attach signed reviewer approvals, protocol identifiers, and dated evidence records.
