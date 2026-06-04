# Clinical Acceptance Review Package

## Purpose
This package defines the 0.4.5 software/QMS review bundle for qualified expert review before any clinical-test authorization activity.

## Scope
The target state is `ClinicalTestAcceptanceReviewReady`: software/QMS evidence is coherent enough for expert preclinical, safety, cybersecurity, physician, and regulatory review. It is not `ClinicalTestReady` and does not authorize human testing.

## Included evidence
- Software freeze report: `qms/generated/ClinicalSoftwareFreezeReport.md`.
- Evidence index: `qms/generated/EvidenceIndex.json`.
- Evidence manifest: `qms/generated/EvidenceManifest_0.4.5.md`.
- HIL evidence: `qms/hil/session_reports/HIL-SESSION-0.4.3.md`, `qms/hil/latency_reports/HIL-LATENCY-0.4.3.md`, and `qms/hil/risk_coverage/HIL-RISK-COVERAGE-0.4.3.csv`.
- Runtime evidence: `qms/generated/Runtime10kBoundedEvidence.md` and `qms/generated/Runtime10kFullEvidence.md`.
- Lane transcripts: `qms/generated/ReadinessLaneTranscript-Bounded.md` and `qms/generated/ReadinessLaneTranscript-Full10k.md`.
- Cybersecurity evidence package: `qms/security/*0.4.4.md`.
- Traceability and verification: `qms/generated/TraceabilityReport.md` and `qms/generated/VerificationIndex.md`.
- Pending sign-off packets: `qms/signoff/SO-*-0.4.5.md`.

## Review decision boundary
A reviewer may conclude that the software/QMS package is ready for clinical-test acceptance review. A reviewer must not conclude that clinical testing is authorized without physical HIL evidence, phantom qualification, named sign-offs, protocol approval, regulatory authorization, and REB/IRB approval.
