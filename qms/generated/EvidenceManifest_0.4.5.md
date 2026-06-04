# EvidenceManifest 0.4.5

## Purpose
This manifest is the explicit QMS evidence-selection source for the 0.4.5 readiness package. Generators must read `qms/generated/EvidenceIndex.json` rather than guessing the newest file by modification time.

## Selection rule
- Evidence paths are selected by stable manifest key.
- File modification time is not a valid QMS selection rule.
- Reused evidence must include a justification and remain blocked for clinical claims until reviewed and signed by named qualified reviewers.

## Indexed artifacts
- HIL session report: `qms/hil/session_reports/HIL-SESSION-0.4.3.md`.
- HIL risk coverage: `qms/hil/risk_coverage/HIL-RISK-COVERAGE-0.4.3.csv`.
- HIL latency report: `qms/hil/latency_reports/HIL-LATENCY-0.4.3.md`.
- Runtime bounded evidence: `qms/generated/Runtime10kBoundedEvidence.md`.
- Runtime full 10k evidence: `qms/generated/Runtime10kFullEvidence.md`.
- Cybersecurity package: `qms/security/*0.4.4.md`.

## Reuse justification
The 0.4.5 package reuses the 0.4.3 deterministic software HIL reports because the HIL implementation and scenario mapping are unchanged in this evidence update. This reuse does not represent physical HIL execution and remains subject to named independent sign-off.

## External blockers
- Qualified physical HIL bench execution.
- Phantom/simulator qualification.
- Target hardware latency and raw log archive.
- Named safety, risk, cybersecurity, physician, and regulatory sign-offs.
- Canada ITA / US IDE and REB/IRB authorization workflow.
