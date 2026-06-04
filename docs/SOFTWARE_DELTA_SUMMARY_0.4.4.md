# Software Delta Summary 0.4.4

## Summary
Version 0.4.4 moved the project from scaffolding toward an evidence-gated software/QMS package suitable for expert preclinical review preparation.

## Key software/QMS deltas
- EvidenceRepository is connected to readiness assessors.
- ReviewRecord, SignOffRecord, and ReviewGate are present for QMS gating.
- Runtime10kFullEvidence and Runtime10kBoundedEvidence are tracked as separate evidence classes.
- ClinicalSoftwareFreezeReport summarizes software-complete status and external blockers.
- Cybersecurity evidence package 0.4.4 adds SBOM, threat-model, vulnerability, key-revocation, secure-update, and disposition records.

## Current boundary
The software/QMS package can support clinical-test acceptance review preparation. It cannot authorize or advertise clinical testing.
