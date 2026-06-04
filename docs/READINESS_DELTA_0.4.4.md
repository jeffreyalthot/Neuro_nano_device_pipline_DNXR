# Readiness Delta 0.4.4

## Improvements
- EvidenceRepository promotion path.
- ReviewGate and SignOffRecord blockers for unsigned evidence.
- Runtime10kFullEvidence availability.
- ClinicalSoftwareFreezeReport availability.
- Cybersecurity evidence package 0.4.4.

## Remaining blockers
- Manifest-driven evidence selection was still needed to prevent modification-time selection errors.
- Bounded and full runtime lane transcripts needed separation.
- Sign-off packets needed named reviewer fields and pending status.
- Physical HIL, phantom qualification, Canada ITA / US IDE, and REB/IRB authorization remained external blockers.

## Target next level
`ClinicalTestAcceptanceReviewReady` should sit between `ClinicalSubmissionReady` and `ClinicalTestReady` to distinguish expert review preparation from actual clinical-test authorization.
